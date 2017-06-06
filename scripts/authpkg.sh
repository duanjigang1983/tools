#!/bin/sh
TOPDIR="/data/web/easepkg"
PKGDIR="/data/gingko"
DB="/usr/bin/mysql -hlocalhost -ucme -pcmemysql --connect_timeout=3  easepkg"
TMPDIR=/tmp/data

##To check new uploads
function check_new_uploads()
{
    mkdir -p "$TMPDIR"
    tmpfile="$TMPDIR/up.txt"
    ## export into file
    strsql="select id, owner, repo,fpath from t_uploads where status='new';"    
    $DB -Ne "$strsql" > "$tmpfile" 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo  1
        return 
    fi
    ##check file
    if ! [ -f "$tmpfile" ]
    then
        echo 2
        return 
    fi
    cat $tmpfile | awk 'NF==4{print $0}' |  while read id owner repo fpath

    #cat $tmpfile  |  while read id owner repo fpath
    do
        ts=`/bin/date +"%Y-%m-%d %H:%M:%S"`
        reason="ok"
        newfpath="$TOPDIR"/$fpath
        if ! [ -f "$newfpath" ]
        then
            bm=`/bin/basename $newfpath`
            reason="ERR: can not find new upload file:$bm"
        else
           ftype=`/usr/bin/file $newfpath | awk '{print $2}'` 
           if [ "$ftype" != "RPM" ]
           then
                msg=`/usr/bin/file $newfpath | awk '{for(i=2;i<=$NF;i++)printf("%s ", $i)}'`
                reason="ERR:error file type:$msg"
           fi
        fi #end of check file type
        ##firstlly, manage error files
        if [ "$reason" != "ok" ]
        then
            strsql="update t_uploads set status='rejected',ret_msg='$reason', last_mod='$ts'  where id = $id;"
            $DB -e "$strsql" > "/dev/null"
            continue
        fi
        ## start to parse rpm file
        ifile="$TMPDIR/rpminfo.txt"
        /bin/rpm -qpi "$newfpath" > "$ifile" 2>/dev/null
        name=`/bin/cat $ifile | grep Name | awk '{print $3}'`
        version=`/bin/cat $ifile | grep Version | awk '{print $3}'`
        release=`/bin/cat $ifile | grep Release | awk '{print $3}'`
        size=`/bin/cat $ifile | grep Size | awk '{print $3}'`
        s1=`dirname $newfpath`
        arch=`basename $s1`
        s2=`dirname $s1`
        osv=`basename $s2` 
        echo "$owner $repo $name $version $release $size $arch $osv  $newfpath"
        md5=`md5sum $newfpath | awk '{print $1}'`
        newdir="$PKGDIR/$owner/$repo/$osv/$arch/$name"
        mkdir -p "$newdir" 2>/dev/null
        if ! [ -d "$newdir" ]
        then
            strsql="update t_uploads set status='rejected', last_mod='$ts', ret_msg='create storage dir for file failed' where id = $id;"
            $DB -e "$strsql" > "/dev/null"
            continue
        fi
        newname="$newdir/`/bin/basename $newfpath`"
        /bin/mv -f  "$newfpath" "$newname" 2>/dev/null
        #/bin/cp -f  "$newfpath" "$newname" 2>/dev/null
        if ! [ -f "$newname" ]
        then
             strsql="update t_uploads set status='rejected',last_mod=`$ts`, ret_msg='move file to storage dir failed' where id = $id;"
            $DB -e "$strsql" > "/dev/null"
            continue
        fi
        strsql="update t_uploads set status='buffer',last_mod='$ts', ret_msg='buffer file success',npath='$newname' where id = $id;"
        $DB -e "$strsql" > "/dev/null"

        ##store into package table
        strsql_del="update t_pkg set status='destroyed' where name='$name' and ver='$version' and rel='$release';"
        strsql_add="insert into t_pkg(name, ver, rel, owner,repo,osv,arch,md5sum,storepath)values('$name', '$version', '$release', '$owner', '$repo', '$osv', '$arch','$md5', '$newname');"
        echo "$strsql_del"
        echo "$strsql_add"
        $DB -e "$strsql_del" > "/dev/null"
        $DB -e "$strsql_add" > "/dev/null"

        echo "BUFFER-OK:$id $owner $repo $fpath"
    done
    ## read file
    echo 0
    return 
}
function update_repo()
{
    strsql="select id, npath from t_uploads where status='buffer';"
    tmpfile="$TMPDIR/buffer.txt"
    $DB -Ne "$strsql" > "$tmpfile" 2>/dev/null
    repofile="$TMPDIR/repo.txt"
    if [ -f "$repofile" ]
    then
        unlink "$repofile"
    fi
    cat $tmpfile | awk 'NF==2{print $1" "$2}' | while read id name
    do
        ts=`/bin/date +"%Y-%m-%d %H:%M:%S"`
        if ! [ -f "$name" ]
        then
            strsql="update t_uploads set status='lost',last_mod='$ts', ret_msg='buffer file lost' where id = $id;"
            $DB -e "$strsql" > "/dev/null"
            continue
        fi
        s1=`/usr/bin/dirname $name`
        s2=`/usr/bin/dirname $s1`
        echo "$s2" >> $repofile
    done

    ##update repo
    if ! [ -f "$repofile" ]
    then
        return 
    fi
    cat $repofile | sort | uniq | while read s2
    do
        ts=`/bin/date +"%Y-%m-%d %H:%M:%S"`
        ###create repo
        /usr/bin/createrepo --update "$s2" 2>&1>/dev/null
        ##update status
        strsql="update t_uploads set status='accept',last_mod='$ts', ret_msg='update repo for $s2 success' where status='buffer' and   position('$s2' in npath) >= 0;"
        $DB -e "$strsql" > "/dev/null"
        echo "REPO-OK: $s2"
        sleep 0.001
    done
}
check_new_uploads
update_repo
