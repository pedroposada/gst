<?php


/**
 * Migration class for Issue Nodes
 */
class NodeIssueMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS Issue Nodes');
    $this->dependencies = array('User', 'File');
    
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'nid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique Node ID',
          'alias' => 'n',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );
    
    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $query = Database::getConnection('default', 'd6')->select('node', 'n');
    $query->fields('n', array('nid', 'vid', 'type', 'language', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'));
    $query->condition('n.type', 'content_issue', '=');
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');
    // body
    $query->addField('nr', 'body');
    $query->addField('nr', 'teaser');
    $query->addField('nr', 'format');
    $query->orderBy('n.changed');
    
    $source_fields = array(
      'nid' => t('The node ID of the article'),
      'title' => t('Headline.'),
      'body' => t('Description.'),
      'teaser' => t('Shorter version of body.'),
      'photo' => t('Include a Photo field.'),
      'field_issue_date_d6' => t('Include an Issue date.'),
    );
    
    $this->highwaterField = array(
      'name' => 'changed',    // Column to be used as highwater mark
      'alias' => 'n',           // Table alias containing that column
      'type' => 'int',          // By default, highwater marks are assumed to be lexicographically
                                // sortable (e.g., '2011-05-19 17:53:12'). To properly
                                // deal with integer highwater marks (such as UNIX
                                // timestamps), indicate so here.
    );
    
    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
    
    // Set up our destination - node in this case.
    $this->destination = new MigrateDestinationNode('issue');
    
    // Assign mappings TO destination fields FROM source fields.
    // $this->addFieldMapping('is_new')->defaultValue(TRUE); // sets node id same as D6 nid
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('uid', 'uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('revision_uid', 'uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('promote', 'promote');
    $this->addFieldMapping('sticky', 'sticky');
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('field_issue_date', 'field_issue_date_d6');
    // $this->addFieldMapping('path', 'url_alias');
	$this->addFieldMapping('field_issue_id', 'title');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
     $this->addFieldMapping('field_image', 'photo')
      ->arguments(array('file_function'=>'file_link','preserve_files' => TRUE))
      ->defaultValue('');
    // simple mappings
    // $this->addSimpleMappings(array('field_issue_date'));
    // $this->addSimpleMappings(array('path'));
    
    // body
    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'), array('source_field' => 'plain_text'));
    $this->addFieldMapping('body', 'body')->arguments($body_arguments);
    
    // Unmapped source fields
    $this->addUnmigratedSources(array('vid','type','language','moderate','tnid','translate', 'teaser','format', 'status'));
    
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new','nid','scheduler', 'path'));
    
    // Disable application of pathauto during migration
    $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
    // load issue fields     
	
    $query = Database::getConnection('default', 'd6')->select('content_type_content_issue', 'ctc');
    $query->fields('ctc');
    $query->condition('ctc.nid', $current_row->nid, '=');
    if ($nodefields = $query->execute()->fetchObject()) {
	
      // issue date
	  //dd($nodefields);
      // $current_row->field_data_field_issue_date = $nodefields->field_issue_date_value;//
      // issue number
	  // $current_row->url_alias = 'issue/' . $nodefields->field_issue_number_value;
	  
	  if ($uri = gs_migrate_mappedfileuri($nodefields->field_cover_graphic_fid)) {
        // prepare content for field_image
        $image = array(
          'path' => $uri,
          'alt' => '',
          'title' => '',
        );
        $current_row->photo = drupal_json_encode($image);
      }
	}
    // Set the url alias for the node.
	// // // // $current_row->url_alias = gs_migrate_get_url_alias($current_row->nid);
	
    // photo
	/*
    $query = Database::getConnection('default', 'd6')->select('content_field_photo', 'cf');
    $query->condition('cf.nid', $current_row->nid, '=');
    $query->join('files', 'f', 'f.fid = cf.field_photo_fid');
    $query->addField('f', 'fid');
    $fid = $query->execute()->fetchField();
    // dd($fid);
	if ($uri = gs_migrate_mappedfileuri($fid)) {
      // prepare content for field_image
      $image = array(
        'path' => $uri,
        'alt' => '',
        'title' => '',
      );
      $current_row->photo = drupal_json_encode($image);
	  
    }*/
    
    $query = Database::getConnection('default', 'd6')->select('content_field_activation_date', 'cd');
    $query->fields('cd');
	$query->condition('cd.nid', $current_row->nid, '=');
    if ($nodefields = $query->execute()->fetchObject()) {
      // issue date
      $current_row->field_issue_date_d6 = $nodefields->field_activation_date_value;//
    }
	// $current_row->type = 'issue';
    //dd($current_row);
	return TRUE;
  }
    
  public function prepare($entity, stdClass $row) {
  }
 
  public function fields($entity_type, $bundle) {
  }
  
  public function complete($entity, stdClass $row) {
  }
}