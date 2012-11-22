<?php

/**
 * Migration class for user accounts
 */ 
class UserMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS users');
	  // $this->dependencies = array('Role');

    $source_fields = array(
      'uid' => t('User ID'),
      'roles' => t('The set of roles assigned to a user.'),
    );
	
	  // select records from user table
    $query = Database::getConnection('default', 'd6')->select('users', 'u');
    $query->fields('u', array('uid', 'name', 'pass', 'mail', 'created', 'access', 'login', 'status', 'init'));
    $query->condition('u.uid', 1, '>');
    $query->orderBy('u.uid', 'ASC');
    
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'uid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique User ID',
          'alias' => 'u',
        )
      ),
      MigrateDestinationUser::getKeySchema()
    );
	
    // Make the mappings
    // $this->addFieldMapping('uid', 'uid')->description(t('Preserve old D6 User ID.'));
    // $this->addFieldMapping('is_new')->defaultValue(TRUE); // Preserve old D6 User ID
    // $this->addFieldMapping('name', 'name')->dedupe('users', 'name');
    $this->addFieldMapping('name', 'name');
    // $this->addFieldMapping('mail', 'mail')->dedupe('users', 'mail');
    $this->addFieldMapping('mail', 'mail');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('language')->defaultValue('');
    $this->addFieldMapping('theme')->defaultValue('');
    $this->addFieldMapping('signature')->defaultValue('');
    $this->addFieldMapping('signature_format')->defaultValue('plain_text');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('access', 'access');
    $this->addFieldMapping('login', 'login');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('picture')->defaultValue(0);
    $this->addFieldMapping('init', 'init');
    $this->addFieldMapping('timezone')->defaultValue(NULL);
    $this->addFieldMapping('roles', 'roles')->defaultValue(array());
    $this->addFieldMapping('path')->issueGroup(t('DNM'));
	
	  // Disable application of pathauto during migration
    $this->addFieldMapping('pathauto')->defaultValue(0);
         
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new','uid'));
  }

  public function prepareRow($current_row) {
	  // add user roles to record
    $query = Database::getConnection('default', 'd6')->select('users_roles', 'r');
    $query->fields('r', array('rid'));
    $query->condition('r.uid', $current_row->uid, '=');
    $results = $query->execute()->fetchCol();
  	if(array_sum($results) > 2){
  		$editor = user_role_load_by_name('editor');
  	  $current_row->roles[$editor->rid] = $editor->rid;
  	}

    return TRUE;
  }
}



/**
 * Migration class for profile fields
 */
class UserProfile2Migration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS user profiles');
	  $this->dependencies = array('User');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'uid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique User ID',
          'alias' => 'u',
        )
      ),
      MigrateDestinationProfile2::getKeySchema()
    );

   // select records from user table
   $query = Database::getConnection('default', 'd6')->select('users', 'u');
   $query->fields('u', array('uid'));
   $query->condition('u.uid', 1, '>');
   $query->orderBy('u.uid', 'ASC');
    
   // use our own count query
   $count_query = Database::getConnection('default', 'd6')->select('users', 'u');    
   $count_query->condition('u.uid', 1, '>');
   $count_query->addExpression('COUNT(uid)', 'cnt');

	 $source_fields = array(
      'uid' => t('User ID'),
      'profile_bio' => t('Bio plus teaser.'),
      'profile_twitter_username' => t('Twitter Username.'),
      'profile_google_profile' => t('Google Profile URL.'),
      'profile_legal' => t("I have read and understood and agreed to Straight.com's Privacy Policy and Terms and Conditions."),
      'profile_postcode' => t('Postal or Zip Code.'),
      'profile_country' => t('Country.'),
      'profile_firstname' => t('First Name.'),
      'profile_lastname' => t('Last Name.'),
      'profile_gender' => t('Gender.'),
      'profile_age' => t('Age.'),
      'profile_notify' => t('Do not notify me by email of replies to my comment.'),
    );

    // Migrate source and destination
    $this->source = new MigrateSourceSQL($query, $source_fields, $count_query, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationProfile2('profile');

	  // Mapped fields
    $this->addFieldMapping('uid','uid')->sourceMigration('User')->defaultValue(1);
	  $this->addFieldMapping('revision_uid', 'uid')->sourceMigration('User');
    $this->addFieldMapping('field_first_name','profile_firstname')->defaultValue('');
    $this->addFieldMapping('field_last_name','profile_lastname')->defaultValue('');
    $this->addFieldMapping('field_twitter_username','profile_twitter_username')->defaultValue('');
    $this->addFieldMapping('field_google_profile','profile_google_profile')->defaultValue('');
    $this->addFieldMapping('field_legal','profile_legal')->defaultValue(1);
    $this->addFieldMapping('field_postcode','profile_postcode')->defaultValue('');
    $this->addFieldMapping('field_country','profile_country')->defaultValue('');
    $this->addFieldMapping('field_gender','profile_gender')->defaultValue('');
    $this->addFieldMapping('field_age','profile_age')->defaultValue('');
    $this->addFieldMapping('field_notify','profile_notify')->defaultValue(1);

    $bio_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'profile_bio_teaser'), array('source_field' => 'format'));
    $this->addFieldMapping('field_bio_teaser', 'profile_bio')->arguments($bio_arguments);

    // Unmapped source fields
    // $this->addUnmigratedSources(array());
 
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('language','path'));
  }

	public function prepareRow($current_row){
		// Set the text format for the node.
  	$current_row->format = gs_migrate_get_text_format(0);
		// add profile fields to record
    $query = Database::getConnection('default', 'd6')->select('profile_fields', 'pf');
    $query->fields('pf', array('name'));
    $query->fields('pv', array('value'));
    $query->join('profile_values','pv','pv.fid = pf.fid AND pv.uid = :uid',array(':uid' => $current_row->uid));
    $results = $query->execute();
    foreach ($results as $row) {
  		if ($row->name == "profile_legal" && !ctype_digit($row->value)) {
  			$row->value = 1;
  		}
  		if ($row->name == "profile_notify" && !ctype_digit($row->value)) {
  			$row->value = 1;
  		}
	    $current_row->{$row->name} = $row->value;
    }
		
		return TRUE;
	}
  
}