<?php


/**
 * Migration class for Poll Nodes
 */
class NodePollMigration extends GSMigration {
	public function __construct() {
		parent::__construct();
		$this->description = t('Migrate GS Poll Nodes');
		$this->dependencies = array('User');
		
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
		$query->condition('n.type', 'poll', '=');
		$query->join('users', 'u', 'n.uid = u.uid');
		$query->addField('u', 'name');
		$query->orderBy('n.changed');
		
		$source_fields = array(
		  'nid' => t('The node ID of the article'),
		  'title' => t('Headline.'),
		  'url_alias' => t('The url alias of the node.'),
		  'active_status' => t('Active status of the poll.'),
		  'seconds_to_run' => t('Time left for the poll.'),
		  'src_choices' => t('List of choices for the poll.'),
		  'src_votes' => t('List of vote results for the poll.'),
		);
		
		$this->highwaterField = array(
		  'name' => 'changed', 		// Column to be used as highwater mark
		  'alias' => 'n',           // Table alias containing that column
		  'type' => 'int',          // By default, highwater marks are assumed to be lexicographically
		                            // sortable (e.g., '2011-05-19 17:53:12'). To properly
		                            // deal with integer highwater marks (such as UNIX
		                            // timestamps), indicate so here.
		);
		
		// use our own count query
		$count_query = Database::getConnection('default', 'd6')->select('node', 'n');    
		$count_query->condition('n.type', 'poll', '=');
		$count_query->addExpression('COUNT(n.nid)', 'cnt');
		
		// Create a MigrateSource object, which manages retrieving the input data.
		$this->source = new MigrateSourceSQL($query, $source_fields, $count_query, array('map_joinable' => FALSE));
		
		// Set up our destination - node in this case.
		$this->destination = new MigrateDestinationNode('poll');
		
		// Assign mappings TO destination fields FROM source fields.
		// $this->addFieldMapping('is_new')->defaultValue(TRUE); // sets node id same as D6 nid
		$this->addFieldMapping('title', 'title');
		// $this->addFieldMapping('nid', 'nid');
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
		
		$this->addFieldMapping('active','active_status')->defaultValue(1);
		$this->addFieldMapping('runtime', 'seconds_to_run');
		$this->addFieldMapping('choice', 'src_choices')->description('src_choices populated in prepareRow()');
		$this->addFieldMapping('votes', 'src_votes')->description('src_votes populated in prepareRow()');
		
		// body
		// $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'), array('source_field' => 'format'));
		// $this->addFieldMapping('body', 'body')->arguments($body_arguments);
		
		// Unmapped source fields
		$this->addUnmigratedSources(array('vid','type','language','moderate','tnid','translate','name',));
		
		// Unmapped destination fields
		$this->addUnmigratedDestinations(array('is_new','nid'));
		
		// Disable application of pathauto during migration
		$this->addFieldMapping('pathauto')->defaultValue(0);
	}
  
  
  
	public function prepareRow($current_row) {
		
		// get the choices
		$current_row->src_choices = array();
		$query = Database::getConnection('default', 'd6')->select('poll_choices','pc');
		$query->fields('pc');
		$query->condition('pc.nid',$current_row->nid,'=');
		$choices = $query->execute();
		foreach ($choices as $choice) {
		    $current_row->src_choices[] = array(
		      'chtext' => $choice->chtext,
		      'chvotes' => $choice->chvotes,
		      'weight' => $choice->chorder,
		    );
		}
		
		// Note that we won't know until much later what the chid is for each
  		// choice, so it's best to tie the votes to choices by text.
		$current_row->src_votes = array();
		$query = Database::getConnection('default', 'd6')->select('poll_choices','pc');
		$query->fields('pc',array('chtext'));
		$query->condition('pc.nid',$current_row->nid,'=');
		$query->join('poll_votes','pv','pc.chorder = pv.chorder');
		$query->addField('pv','hostname');
		$query->addField('pv','uid');
		$query->condition('pv.nid',$current_row->nid,'=');
		$query->orderBy('pv.hostname');
		$query->addExpression('CONCAT(pc.nid, pv.uid, pv.hostname)','pk');
		$query->groupBy('pk');
		$votes = $query->execute();
		foreach ($votes as $vote) {
			$hostname = !empty($vote->hostname) ? $vote->hostname : '127.0.0.1';
		    $current_row->src_votes[] = array(
		      'chtext' => $vote->chtext,
		      'uid' => $vote->uid,
		      'hostname' => $hostname,
		      'timestamp' => 0,
		    );
		}
		
		// get runtime and active status
		$query = Database::getConnection('default', 'd6')->select('poll','p');
		$query->fields('p',array('runtime','active'));
		$query->condition('p.nid',$current_row->nid,'=');
		$poll = $query->execute()->fetchObject();
		$current_row->seconds_to_run = $poll->runtime;
		$current_row->active_status = $poll->active;
		
		return TRUE;
	}
	  
	public function prepare($entity, stdClass $row){
	}
 
	public function fields($entity_type, $bundle) {
		// MigratePollEntityHandler::fields($entity_type, $bundle);
  	}
	
	public function complete($entity, stdClass $row){
		// MigratePollEntityHandler::complete($entity, $row);
	}
}

