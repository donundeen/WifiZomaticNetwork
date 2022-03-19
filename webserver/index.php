<?php
ini_set('display_errors', 1);

$debug_messages = array();
$user_messages = array();

$fart_array = [
	"/home/pi/WifiZomaticNetwork/audio/fs1.wav"
];

$fart = $fart_array[array_rand($fart_array)];

if($_GET['fart']){
exec("/usr/bin/aplay $fart 2>&1", $output, $retval);

}

?>
<html>
<head>
<title>WifiZomatic Network</title>
<style>
body {
  font-size:5vw;
}
h1 {
  font-size:10vw;
}

h2 {
  font-size:8vw;
}

h3 {
  font-size:7vw;
}

input, button,label, textarea {
  font-size:6vw;
}

.usermessages {
 color:green;
}

.responses{
  font-size: 4vw;
}



.rowquestion{
  font-style: italic;
  font-size: 5vw;
}

.rowloginname {
  font-weight: bold;
  font-size: 5vw;
}

.rowresponse {
  font-style: italic;
  font-size: 5vw;
}

.rowdatetime{
  font-size:.4em;
}

.askquestion{
	font-size:1.1em;
	font-weight: bold;
}

.wontshare{
  font-size:.4em;
  font-style: italic;

}

textarea{
  width: 100%;
  font-family:serif;
}

.intro {
  text-align:center;
}

.fart{
font-size:10em;
}
</style>
</head>
<body>
<!--
<pre>
<?php print implode($debug_messages,"<br/>\n"); ?>
</pre>
-->
<p class="usermessages">
<?php print implode($user_messages,"<br/>\n"); ?>
</p>
<h1>Hi there, I'm Log, thanks for Logging in!</h1>
<center>
<span class="intro">
<a class="fart" href="index.php?fart=true">💩</a>
<br/>
<a class="farttext" href="index.php?fart=true">Drop a Log</a>
</span>

</center>

</html>
