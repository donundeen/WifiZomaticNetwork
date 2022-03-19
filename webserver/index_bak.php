<?php
ini_set('display_errors', 1);

$current_table = "LOGv1";

$debug_messages = array();

$user_messages = array();

$question_array = [
	"What else could I be?",
	"Who do you think I am?",
	"What would make you like me?",
	"How could I be funnier?",
	"What would be fun to do together?"
];

class LogDB extends SQLite3 {
      function __construct() {
         $this->open('log.db');
      }
}

$db = new LogDB();
if(!$db) {
   $messages[] = $db->lastErrorMsg();
} else {
   $messages[] =  "Opened database successfully\n";
}

$sql =<<<EOF
      CREATE TABLE IF NOT EXISTS $current_table
      (
      LOGINNAME TEXT,
      QUESTION  TEXT NOT NULL,
      QUESTIONRESPONSE TEXT,
      DATETIME	TEXT NOT NULL);
EOF;

$ret = $db->exec($sql);
if(!$ret){
   $messages[] =  $db->lastErrorMsg();
} else {
   $messages[] = "Table created successfully\n";
}

$loginname = "";
if($_POST){
	$messages[] = "Got form submit";
	$messages[] = $_POST['loginname'];
	$messages[] = $_POST['questionresponse'];
	$messages[] = $_POST['question'];

	$loginname = SQLite3::escapeString($_POST['loginname']);
	$questionresponse = $db->escapeString($_POST['questionresponse']);
	$question  = $db->escapeString($_POST['question']);

	$sql = "INSERT INTO $current_table (LOGINNAME, QUESTION, QUESTIONRESPONSE, DATETIME) "
	     .	"VALUES ('$loginname','$question','$questionresponse',datetime('now'))";
	$messages[] = $sql;

	$ret = $db->exec($sql);
	if(!$ret){
		$messages[] = "some error";
   		$messages[] =  $db->lastErrorMsg();
	} else {
		$messages[] = "data inserted successfully\n";
	}
	$user_messages[] = "Thanks for that, $loginname! I've logged your words to my log. This stays right here on the local log.";

}

$selectquery = "SELECT * FROM $current_table ORDER BY DATETIME DESC";

$foo = "bar";
$question = $question_array[array_rand($question_array)];


?>
<html>
<head>
<title>Log</title>
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
<span class="intro">
I'm a very local log. I don't talk to the internet; everything you see here stems from my personal log, in my in-tree-net. <BR>
I'm trying to be a better Login Log. I'm going to grow, change, and branch out with your help. 
<BR>
<BR>
<span class="askquestion">Can I ask you a question? </span>
<BR/>
<span class="wontshare">(I won't share this online, but people can visit me and see what you wrote on this b-log.)<span>
</span>

<form action="index.php" method="post">
<label for="loginname">What's your (fake) Login Name?</label><br>
<input type="text" id="loginname" size="20" name="loginname" value="<?php print htmlspecialchars($loginname, ENT_QUOTES); ?>">
<br/>
<label for="questionresponse"><?php print $question?></label><br/>
<textarea name="questionresponse" id="questionresponse" rows=2></textarea>
<input type="hidden" name="question" value="<?php print(htmlspecialchars($question, ENT_QUOTES));?>">
<br/>
<BR>
<button type="submit" class="submitbutton">Log your login and response toLog's log in the log</button>
</form>
<p class="responses">
<h2>What I've logged so far</h2>
<?php
$ret = $db->query($selectquery);
if(!$ret){
	print("um, oops:". $db->lastErrorMessage());
}else{
   while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
      print "<span class='rowquestion'>Q: ". $row['QUESTION'] . "</span><br/>";
      print  "<span class='rowloginname'>".$row['LOGINNAME'] ."</span>: ";
      print "<span class='rowresponse'> ". $row['QUESTIONRESPONSE'] ."</span><BR>";
      print "<span class='rowdatetime'>(logged at ".$row['DATETIME'] .")</span><BR><hr>";
   }
   $db->close();
}
?>
</p>
</body>
<?php
$db->close();
?>
</html>
