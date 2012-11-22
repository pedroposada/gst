This module provides menu handlers to serve rss and xml as well as process POST and GET requests from Conmio server.


Instructions:
-------------
- Enable the module as usual, admin/modules "GS Conmio".



Notes:
------
- List of handlers:
  
  api/isawyou
  Handles creation of I Saw You node. Returns json array with success and/or error messages.
  Test from browser:
  http://www.straight.com/api/isawyou?uid=454&title=TESTINGS&location=LOCATION%20TESTINGS&description=BODY%20DESCRIPTION&i_am_gender=Male&i_saw_gender=Female&date=2012-11-23
  
  timeout/rss/export/isawyou
  Returns list of I Saw You posts in xml format. 
  Test from browser:
  http://www.straight.com/timeout/rss/export/isawyou
  
  timeout/rss/export/search/articles/SEARCHTERM
  Returns list of articles in xml format. SEARCHTERM is the term user entered in mobile.
  Test from browser:
  http://www.straight.com/timeout/rss/export/search/articles/SEARCHTERM
  
  api/confession
  Handles creation of Confession node. Returns json array with success and/or error messages.
  Test from browser:
  http://www.straight.com/api/isawyou?uid=454&title=TESTINGS&description=BODY%20DESCRIPTION
  
  api/auth
  Handles authentication of user. Returns json array with success and/or error messages and user object.
  Expects POST request
  Test from terminal:
  curl -d "name=YOURUSERNAME&pass=YOURPASSWORD" http://www.straight.com/api/auth
  
  api/user_create
  Handles creation of user account. Returns json array with success and/or error messages and user object.
  Expects POST request
  Test from terminal:
  curl -d "name=YOURUSERNAME&pass=YOURPASSWORD&pass_confirm=YOURPASSWORD&email=YOUREMAIL&email_confirm=YOUREMAIL" http://www.straight.com/api/user_create
  
  api/comment
  Handles request to create a comment. Returns json array with success and/or error messages.
  Test from terminal:
  User not logged in to mobile:
  curl -d "author=Hobo Rick&comment=badger badger badger&nid=330345&ip_address=1.2.3.4" http://www.straight.com/api/comment
  User logged in to mobile:
  curl -d "uid=546&comment=i watch the cat&nid=30345&ip_address=1.2.3.4" http://www.straight.com/api/comment
  
  api/contestEntry
  Handles contest entry request. Returns json array with success and/or error messages.
  
  timeout/rss/export/comments/CID
  Simple redirect to rss/conmio/feed/node/CID
  
  timeout/rss/export/article/NID
  Simple redirect to rss/conmio/feed/comments/NID
	  
	  
