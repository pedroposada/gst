<?php


/**
 * Migration class for Gallery Nodes
 */
class FileMigration extends GSMigration {
  public function __construct() {
  	parent::__construct();
  	$this->description = t('Migrate GS Galleries Files');
  	$this->dependencies = array('User');
  	
  	$this->map = new MigrateSQLMap($this->machineName,
        array(
          'fid' => array(
            'type' => 'int',
            'unsigned' => TRUE,
            'not null' => TRUE,
            'description' => 'Unique File ID',
            'alias' => 'f',
          )
        ),
        MigrateDestinationFile::getKeySchema()
    );
  	
  	// We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    // $subq = db_select('migrate_message_file', 'mf');
    // $subq->fields('mf',array('sourceid1'));
    // $subq->condition('mf.level',1,'=');
    // $subq->condition('mf.message','Could not copy file%','LIKE');
    // $ids = $subq->execute()->fetchCol();
    
    $query = Database::getConnection('default', 'd6')->select('files', 'f');
    $query->fields('f',array('fid','uid','filename','filepath','filemime','filesize','timestamp'));
	
  	$ids = file(drupal_get_path('module', 'gs_migrate').'/fid_skip.txt');
  	if(is_array($ids) && count($ids)) {
    	$query->condition('f.fid',$ids,'NOT IN');
  	}
	
  	$source_fields = array(
  		'fid' => t('File Id.'),
  		'uid' => t('User Id.'),
  		'filename' => t('Name of file.'),
  		'src_filepath' => t('URL to file.'),
  		'filemime' => t('File type.'),
	    'timestamp' => t('Time when file was created in the system.'),
    );
	
    // Create a MigrateSource object, which manages retrieving the input data.
    // $this->source = new MigrateSourceSQL($query, $source_fields, $count_query, array('map_joinable' => FALSE));
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
	
  	// Set up our destination - file in this case.
  	$this->destination = new MigrateDestinationFile(array(
  		'copy_file'=>TRUE,
  		'preserve_files'=>TRUE,
  	), 'file');
  	
  	// Assign mappings TO destination fields FROM source fields.
    // $this->addFieldMapping('fid','fid');
    $this->addFieldMapping('uid','uid')->sourceMigration('User')->defaultValue(1);
    $this->addFieldMapping('filename','filename');
    $this->addFieldMapping('uri','src_filepath');
    $this->addFieldMapping('filemime','filemime');
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('timestamp','timestamp');
	
    // Unmapped source fields
    $this->addUnmigratedSources(array('fid','filepath','filesize'));
	
  	// Unmapped destination fields
  	$this->addUnmigratedDestinations(array('path','contents'));
  	
  	// Disable application of pathauto during migration
    // $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
	
  	$q = Database::getConnection('default', 'd6')->select('variable', 'v');
  	$q->fields('v',array('value'));
  	$q->condition('v.name','file_directory_path','=');
  	$file_directory_path = unserialize($q->execute()->fetchField());
  	
  	$path = substr($current_row->filepath,0,strlen($file_directory_path)) == $file_directory_path ? 
  		str_replace(' ', '%20', variable_get('gs_migrate_d6_files_directory_path').'/'.$current_row->filepath) : 
  		str_replace(' ', '%20', variable_get('gs_migrate_d6_files_directory_path').'/'.$file_directory_path . '/' .$current_row->filepath);
  	
  	$current_row->src_filepath = $path;
	
    return TRUE;
  }
}