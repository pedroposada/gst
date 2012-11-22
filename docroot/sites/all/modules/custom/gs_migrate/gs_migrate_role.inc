<?php

/**
 * Migration class for Roles
 */
class RoleMigration extends GSMigration {
  public function __construct() {
	parent::__construct();
    $this->description = t('Migrate GS roles');

    $source_fields = array(
      'rid' => t('Role ID.'),
      'name' => t('Role name.'),
      'weight' => t('Order it should be displayed in.'),
    );

	// select records from user table
    $query = Database::getConnection('default', 'd6')->select('role', 'r');
    $query->fields('r', array('rid','name'));
	$query->condition('r.rid', 2, '>');
    $query->orderBy('r.rid', 'ASC');
	
    // use our own count query
    $count_query = Database::getConnection('default', 'd6')->select('role', 'r');
	$count_query->condition('r.rid', 2, '>');    
    $count_query->addExpression('COUNT(r.rid)', 'cnt');

    $this->source = new MigrateSourceSQL($query, $source_fields, $count_query, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationRole();

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'rid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique Role ID',
        )
      ),
      MigrateDestinationRole::getKeySchema()
    );

    // Mapped fields
    $this->addFieldMapping('name', 'name');
	
	// Unmapped destination fields
    $this->addFieldMapping('weight')
         ->issueGroup(t('DNM'));
    
  }

	public function prepareRow($current_row) {
		
		$existing_roles = (array)user_roles();
		if(in_array($current_row->name, array_values($existing_roles)))
			return FALSE;
		
		return TRUE;
	}
}