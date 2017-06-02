<?php
session_start();
include_once("pkg_conf.php");
include_once("pkg_func.php");
if(!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW'])) 
{ 
	header("WWW-Authenticate:Basic realm='login first'"); 
	header("HTTP/1.0 401 Unauthorized"); 
	echo "auth failed, you can not access this page"; 
	exit(0); 
}else
{
    $code="gingko-uploader";
	$user =$_SERVER['PHP_AUTH_USER'];
	$pass =$_SERVER['PHP_AUTH_PW'];
	echo $user."-".$pass; 
    print_r($_FILES);
    $uid=  $_GET['uid'];
    $aa= move_uploaded_file($_FILES[$code]["tmp_name"], "/tmp/".$_FILES[$code]['name']);
    if($aa){
      echo "upload file for '$uid' success";
    }
}
?>
