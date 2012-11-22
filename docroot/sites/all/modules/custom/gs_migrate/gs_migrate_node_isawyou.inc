<?php


/**
 * Migration class for I Saw You Nodes
 */
class NodeIsawyouMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS I Saw You Nodes');
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
    $query->condition('n.type', 'i_saw_you', '=');
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');
    // body
    $query->addField('nr', 'body');
    $query->addField('nr', 'teaser');
    $query->addField('nr', 'format');
    $query->orderBy('n.changed');
    
    $source_fields = array(
      'nid' => t('The node ID of the article'),
      'title' => t('Headline.'),
      'url_alias' => t('The url alias of the node.'),
      'body' => t('Description.'),
      'teaser' => t('Shorter version of body.'),
      'photo' => t('Include a Photo field.'),
      'field_i_saw_a' => t('Male or Female.'),
      'field_i_am_a' => t('Male or Female.'),
      'field_where_did_you_see_this_per' => t('Location.'),
      'field_what_date_did_you_see_them' => t('Date.'),
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
    $this->destination = new MigrateDestinationNode('i_saw_you');
    
    // Assign mappings TO destination fields FROM source fields.
    // $this->addFieldMapping('is_new')->defaultValue(TRUE); // sets node id same as D6 nid
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('uid', 'uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('revision_uid', 'uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('promote', 'promote');
    $this->addFieldMapping('sticky', 'sticky');
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_image', 'photo')
      ->arguments(array('file_function'=>'file_link','preserve_files' => TRUE))
      ->defaultValue('');
    // simple mappings
    $this->addSimpleMappings(array('field_i_saw_a','field_i_am_a','field_where_did_you_see_this_per','field_what_date_did_you_see_them'));
    
    // body
    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'), array('source_field' => 'plain_text'));
    $this->addFieldMapping('body', 'body')->arguments($body_arguments);
    
    // Unmapped source fields
    $this->addUnmigratedSources(array('vid','type','language','moderate','tnid','translate', 'teaser','format'));
    
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new','nid','scheduler'));
    
    // Disable application of pathauto during migration
    $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
    // load isawyou fields     
    $query = Database::getConnection('default', 'd6')->select('content_type_i_saw_you', 'ctc');
    $query->fields('ctc');
    $query->condition('ctc.nid', $current_row->nid, '=');
    if ($nodefields = $query->execute()->fetchObject()) {
      // i saw a
      $current_row->field_i_saw_a = $nodefields->field_gender_value;
      // i am a
      $current_row->field_i_am_a = $nodefields->field_gender_of_user_value;
      // where
      $current_row->field_where_did_you_see_this_per = $nodefields->field_location_value;
      // date
      $current_row->field_what_date_did_you_see_them = (string)$nodefields->field_date_seen_value;
    }
    // Set the url alias for the node.
    $current_row->url_alias = gs_migrate_get_url_alias($current_row->nid);
    // photo
    $query = Database::getConnection('default', 'd6')->select('content_field_photo', 'cf');
    $query->condition('cf.nid', $current_row->nid, '=');
    $query->join('files', 'f', 'f.fid = cf.field_photo_fid');
    $query->addField('f', 'fid');
    $fid = $query->execute()->fetchField();
    if ($uri = gs_migrate_mappedfileuri($fid)) {
      // prepare content for field_image
      $image = array(
        'path' => $uri,
        'alt' => '',
        'title' => '',
      );
      $current_row->photo = drupal_json_encode($image);
    }
    
    return TRUE;
  }
    
  public function prepare($entity, stdClass $row) {
  }
 
  public function fields($entity_type, $bundle) {
  }
  
  public function complete($entity, stdClass $row) {
  }
}