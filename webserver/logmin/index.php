<?php
// ADMIN  PAGE

ini_set('display_errors', 1);
error_reporting(E_ALL & ~E_NOTICE);

$current_table = "LOGv1";

$debug_messages = array();

$user_messages = array();

$question_array = [
        "What else could I be?",
        "Who do you think I am?",
        "What would make you like me?",
        "What could we do together?"
];


class LogDB extends SQLite3 {
      function __construct() {
         $this->open('../log.db');
      }
}

$db = new LogDB();
if(!$db) {
   $messages[] = $db->lastErrorMsg();
} else {
   $messages[] =  "Opened database successfully\n";
}


if($_POST['delete_table'] == 'true'){
   $sql = "DELETE TABLE $current_table";
        $ret = $db->exec($sql);
        if(!$ret){
                $messages[] = "some error while deleting";
                $messages[] =  $db->lastErrorMsg();
        } else {
                $messages[] = "table deleted  successfully\n";
        }
}


if($_POST['create_table'] == 'true'){
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
}

if($_GET['delete_row'] == 'true'){
	$sql ="DELETE FROM $current_table WHERE rowid = ".SQLite3::escapeString($_GET['rowid']);
	$debug_messages[] = $sql;
        $ret = $db->exec($sql);
        if(!$ret){
                $messages[] =  $db->lastErrorMsg();
        } else {
                $messages[] = "row deleted successfully\n";
        }
}

if($_POST['new_row'] == 'true'){
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
	$user_messages = "Thanks for that, $loginname, I've logged your message to my log in the Log";

}

$selectquery = "SELECT rowid, * FROM $current_table ORDER BY DATETIME DESC";

$foo = "bar";
$question = $question_array[array_rand($question_array)];


?>
<html>
<head>
<title>Log</title>
<style>
</style>
</head>
<body>

<pre>
<?php print implode("<br/>\n", $debug_messages); ?>
</pre>


<h1>Welcome to Log</h1>
<p class="usermessages">
<?php print implode("<br/>\n", $user_messages); ?>
</p>
<form action="index.php" method="post">
<label for="loginname">What's your (Fake) Login Name?</label><br>
<input type="text" id="loginname" size="20" name="loginname">
<br/>
<label for="questionresponse"><?php print $question?></label><br/>
<input type="text" id="questionresponse" size="60" name="questionresponse">
<input type="hidden" name="question" value="<?php print(htmlspecialchars($question, ENT_QUOTES));?>">
<br/>
<input type="submit" value="Log your login and response to Log's log in the log">
</form>
<div class="logoflogins">
<?php
$ret = $db->query($selectquery);
if(!$ret){
	print("um, oops:". $db->lastErrorMessage());
}else{
   while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
      print "Q: ". $row['QUESTION'] . "<br/>";
      print  $row['LOGINNAME'] .": ";
      print " ". $row['QUESTIONRESPONSE'] ."<BR>";
      print "(".$row['DATETIME'] .")<BR>";
      print " Row: ". $row['rowid']."<BR>";
      print "<a href='?delete_row=true&rowid=".$row['rowid']."'>Delete Entry</a><hr>";
   }
   $db->close();
}
?>
</div>
</body>
<?php
$db->close();
?>
</html>
