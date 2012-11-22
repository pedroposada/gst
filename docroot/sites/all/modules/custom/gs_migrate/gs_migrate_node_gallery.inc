<?php

/**
 * Migration class for Gallery Nodes
 */
class NodeGalleryMigration extends GSMigration {
  public function __construct() {
  	parent::__construct();
  	$this->description = t('Migrate GS Galleries');
  	$this->dependencies = array('File');
  	
  	$this->map = new MigrateSQLMap($this->machineName,
      array(
        'tid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Unique Node ID',
          'alias' => 'td',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );
  	
  	// We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $query = Database::getConnection('default', 'd6')->select('term_data', 'td');
    $query->fields('td', array('tid', 'name', 'description'));
    $query->condition('td.vid', 6, '='); // "Galleries" vocabulary id
    $query->condition('td.tid', array('196437'), 'NOT IN'); // "Ad-Hoc Image" gallery
    $query->join('term_node','tn','tn.tid = td.tid');
    $query->groupBy('td.tid');
  	
  	$source_fields = array(
    	'tid' => t('Term Id.'),
    	// 'uid' => t('User Id.'),
    	// 'vid' => t('Vocabulary Id.'),
    	'description' => t('Term description.'),
    	// 'weight' => t('Term weight.'),
      'name' => t('Gallery Name.'),
      // 'misc_images' => t('Multiple images from nodes tagged with this term.'),
      'most_recent_image_created' => t('UNIX time stamp from the most recent image in the gallery.'),
      'most_recent_image_uid' => t('User ID from the most recent image in the gallery.'),
      'byline' => t('Byline user id value.'),
    );
  	
  	// Create a MigrateSource object, which manages retrieving the input data.
    // $this->source = new MigrateSourceSQL($query, $source_fields, $count_query, array('map_joinable' => FALSE));
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
  	
  	// Set up our destination - node in this case.
    $this->destination = new MigrateDestinationNode('gallery');
  	
  	// Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('uid','most_recent_image_uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('revision_uid','most_recent_image_uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('title', 'name');
    $this->addFieldMapping('created', 'most_recent_image_created');
    $this->addFieldMapping('field_byline', 'byline')->sourceMigration('User')->defaultValue(1);
  	
  	$body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'description'), array('source_field' => 'format'));
      $this->addFieldMapping('body', 'description')->arguments($body_arguments);
  	
  	// Unmapped source fields
    $this->addUnmigratedSources(array('tid'));
  	
  	// Unmapped destination fields
  	$this->addUnmigratedDestinations(array('scheduler','field_mediacollection', 'is_new','nid','changed','status','promote', 'sticky', 
  		'comment', 'path', 'revision', 'language'));
  	
  	// Disable application of pathauto during migration
    $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
	
  	// Set the correct text format for the comment.
    $current_row->format = gs_migrate_get_text_format(0);
	
    // get date and uid of most recent image in gallery
    $query = Database::getConnection('default','d6')->select('term_node','tn');
    $query->fields('tn', array('tid'));
    $query->join('content_type_content_gallery_image','gn','tn.nid = gn.nid');
    // $query->addField('gn', 'field_display_image_data');
    $query->join('files','f','f.fid = gn.field_display_image_fid');
    $query->join('node','n','n.nid = gn.nid');
    // $query->addField('f', 'filepath');
    // $query->addField('f', 'fid');
    // $query->addField('f', 'uid');
    // $query->addField('f', 'timestamp');
    $query->addField('n', 'created');
    $query->addField('n', 'uid');
    // $query->condition('tn.tid',$tids,'IN');
    $query->condition('tn.tid',$current_row->tid,'=');
    // $query->groupBy('f.fid');
    $query->orderBy('n.created','DESC');
    $query->range(0,1);
    $result = $query->execute()->fetchObject();
    $current_row->most_recent_image_created = $result->created;
    $current_row->most_recent_image_uid = $result->uid; 
    $current_row->byline = $result->uid; 
  
    return TRUE;
  }
}