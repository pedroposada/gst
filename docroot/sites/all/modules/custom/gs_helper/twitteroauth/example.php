<?php
// Insert your keys/tokens
$consumerKey = 'insert-your-consumer-key';
$consumerSecret = 'insert-your-consumer-secret';
$oAuthToken = 'insert-your-oAuthToken';
$oAuthSecret = 'insert-your-oAuthSecret';

// Full path to twitteroauth.php (change oauth to your own path)
require_once($_SERVER['DOCUMENT_ROOT'].'/oauth/twitteroauth.php');

// create new instance
$tweet = new TwitterOAuth($consumerKey, $consumerSecret, $oAuthToken, $oAuthSecret);

// Your Message
$message = "This is a test message.";

// Send tweet 
$tweet->post('statuses/update', array('status' => "$message"));
?>