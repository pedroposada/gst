<?php

/**
 * Migration class for Confession Comments
 */
class CommentIsawyouMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate I Saw You comments.');
    $this->dependencies = array('NodeIsawyou');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'cid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique Comment ID',
          'alias' => 'c',
        )
      ),
      MigrateDestinationComment::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $query = Database::getConnection('default', 'd6')->select('comments', 'c');
    $query->fields('c', array('cid', 'pid', 'nid', 'uid', 'subject', 'comment', 'hostname', 'timestamp', 'status', 'thread', 'name', 'mail', 'homepage', 'format'));
    $query->leftJoin('node', 'n', 'c.nid = n.nid');
    $query->condition('n.type', 'i_saw_you', '=');
    $query->orderBy('c.cid', 'ASC');
  
  $source_fields = array(
      'cid' => t('Comment ID'),
      'pid' => t('Parent ID'),
      'nid' => t('Node ID'),
      'subject' => t('Subject'),
      'comment' => t('Comment'),
      'hostname' => t('Host IP'),
      'timestamp' => t('Creation UNIX time'),
      'status' => t('Active status'),
      'thread' => t('Show with threads.'),
      'name' => t('Name of user.'),
      'mail' => t('E-mail of user.'),
      'homepage' => t('Home page of user.'),
      'format' => t('Input format.'),
    );
  
    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));

    // Set up our destination - comments in this case.
    $this->destination = new MigrateDestinationComment('comment_node_i_saw_you');

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('pid', 'pid')->sourceMigration('CommentIsawyou');
    $this->addFieldMapping('nid', 'nid')->sourceMigration('NodeIsawyou');
    $this->addFieldMapping('uid', 'uid')->sourceMigration('User')->defaultValue(NULL);
    $this->addFieldMapping('subject', 'subject');
    $this->addFieldMapping('hostname', 'hostname');
    $this->addFieldMapping('created', 'timestamp');
    $this->addFieldMapping('changed', 'timestamp');
    $this->addFieldMapping('status')->defaultValue(COMMENT_PUBLISHED);
    $this->addFieldMapping('thread', 'thread');
    $this->addFieldMapping('field_yourname', 'name');
    $this->addFieldMapping('mail', 'mail');
    $this->addFieldMapping('homepage', 'homepage');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);

    $comment_body_arguments = MigrateTextFieldHandler::arguments(NULL, array('source_field' => 'format'));
    $this->addFieldMapping('comment_body', 'comment')->arguments($comment_body_arguments);

    // Unmapped source fields
    $this->addUnmigratedSources(array('format', 'status','cid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('path','name'));
  }

  public function prepareRow($current_row) {
    // Set the correct text format for the comment.
    $current_row->format = gs_migrate_get_text_format($current_row->format);

    return TRUE;
  }
}