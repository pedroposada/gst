This module provides scripts for importing nodes, comments, users, files, taxonomy terms.


Instructions:
-------------
- Enable the module as usual, admin/modules "GS Migrate".

- Visit admin page at "admin/content/migrate"

- 



Notes:
------
- TermSection will reset the structure of the Article vocabulary.
  The Article vocabulary has been re-ordered in D7. 
  If you need to import a new term from D6 to D7, then use drush command:
  
	drush mi TermSection --idlist=D6_TERM_ID
	
  Where D6_TERM_ID is the id of the term in D6 database.
  Once the term has been imported, it can be re-ordered in D7.
  
- 