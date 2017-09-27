<?php

$mysql_host = '192.168.0.76';
$mysql_user = 'root';
$mysql_password = 'issi';
$mysql_db = 'issi';

// Á¢¼Ó
$conn = @mysqli_connect($mysql_host, $mysql_user, $mysql_password, $mysql_db);


$sql="select * from encryptkey order by date desc limit 1";

$res = mysqli_query($conn, $sql);

$result = array();

	while($row = @mysqli_fetch_array($res)){
		array_push($result, array('aes'=>$row[0], 'rsa'=>$row[1], 'rsa2'=>$row[2], 'rsa3'=>$row[3]));
	}

	$json = json_encode(array("result"=>$result));
	echo $json;

	mysqli_close($conn);

?>