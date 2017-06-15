<?php
session_start();
include_once("pkg_conf.php");
include_once("pkg_func.php");
if(!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW'])) 
{ 
	/*	
	header("WWW-Authenticate:Basic realm='login first'"); 
	header("HTTP/1.0 401 Unauthorized"); 
	echo "auth failed, you can not access this page"; 
	exit(0);
	*/	
     $ret = array();
     $ret['result'] = "failed";
     $ret['msg'] = "HTTP/1.0 401 Unauthorized";
	 print_r(json_encode($ret));		
	
}else
{
	
	if(empty($_GET['uid']) || empty($_GET['repo'])||empty($_GET['osv'])||empty($_GET['arch'])||empty($_GET['token']))
	{
		$ret = array();
		$ret['result'] = "failed";
		$ret['msg'] = "inadequent parameters";
		print_r(json_encode($ret));
	}else	
	{
    	$code="gingko-uploader";
		$user =$_SERVER['PHP_AUTH_USER'];
		$pass =$_SERVER['PHP_AUTH_PW'];
	
		//echo $user."-".$pass; 
    	//print_r($_FILES);
    	$uid=  $_GET['uid'];
		$repo = $_GET['repo'];
		$osv = $_GET['osv'];
		$arch = $_GET['arch'];
		$token = $_GET['token'];
		$name = $_FILES[$code]['name'];
		$ret['result'] = 'success';
		$ret['msg'] = 'default success';
		$ret1 = try_upload_file($pkg_db, $user, $pass, $uid, $repo, $osv, $arch, $token, $_FILES, $code);
		print_r(json_encode($ret1));
		/*
    	$aa= move_uploaded_file($_FILES[$code]["tmp_name"], "/tmp/".$_FILES[$code]['name']);
    	if($aa)
		{
			$ret = try_upload_file($user, $pass, $uid,);
			//$ret = array();
			//$ret['result'] = "success";
      		//$ret['msg'] = "upload file '$name' for uid '$uid' success";
			print_r(json_encode($ret));
   	 	}else
		{
			$ret = array();
			$ret['result'] = "failed";
			$ret['msg'] = "try to move file on server failed";
			print_r(json_encode($ret));
	
		}*/
	}//end of parameter check
}
?>
