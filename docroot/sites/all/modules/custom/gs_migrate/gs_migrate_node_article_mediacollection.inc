<?php

/** 
 * TEST QUERY
select f.fid
from node n 
inner join content_field_wide_image cw on cw.nid = n.nid
inner join content_field_inline_image ci on ci.nid = n.nid
inner join content_type_content_article ca on ca.nid = n.nid
inner join files f on 
f.fid = ca.field_central_image_fid 
or f.fid = ca.field_associated_image_fid
or f.fid = ci.field_inline_image_fid
or f.fid = cw.field_wide_image_fid
where n.type = 'content_article'
and n.nid in ( '275844','276149' )
 *
 * fids: 86434,86435
 * /

/**
 * Migration class for mediacollection field in article nodes
 */
class NodeArticleMediacollectionMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS Articles media collection field');
    $this->dependencies = array('NodeArticle','File');
    
    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'fid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Unique File ID',
          'alias' => 'f',
        )
      ),
      MigrateDestinationFieldCollection::getKeySchema()
    );
    
    // QUERY
    $excludeblog = Database::getConnection('default', 'd6')->select('content_type_content_article','t');
    $excludeblog->fields('t', array('nid'));
    $excludeblog->condition('t.field_primary_section_value',explode(',',variable_get('gs_migrate_d6_blog_term_ids')),'NOT IN');
    $excludeblog->groupBy('t.nid');
    
    $subquery = Database::getConnection('default', 'd6')->select('node', 'n');
    $subquery->fields('n', array('nid'));
    $subquery->condition('n.type', 'content_article', '=');
    $subquery->join('node_revisions', 'nr', 'n.vid = nr.vid');
    $subquery->join($excludeblog,'eb','eb.nid = n.nid');
    $subquery->orderBy('n.changed');
    $nids = $subquery->execute()->fetchCol();
    
    $query = Database::getConnection('default', 'd6')->select('node', 'n');
    $query->join('content_field_wide_image', 'cw', 'cw.nid = n.nid');
    $query->join('content_field_inline_image', 'ci', 'ci.nid = n.nid');
    $query->join('content_type_content_article', 'ca', 'ca.nid = n.nid');
    $query->join('files', 'f', 'f.fid = cw.field_wide_image_fid OR f.fid = ci.field_inline_image_fid OR f.fid = ca.field_associated_image_fid OR f.fid = ca.field_central_image_fid');
    $query->addField('f', 'fid');
    $query->addField('n', 'nid');
    $query->condition('n.nid',$nids,'IN');
    
    // source fields
    $source_fields = array(
      'nid' => t('Node ID.'),
      'mediafile' => t('JSON array with Path, Alt and Title for the file.'),
      'filepath' => t('Path of the file.'),
      'fid' => t('Unique ID for the file.'),
      'field_display_image_data' => t('Serialized array with metadata of the file.'),
      'credit' => t('Credit text for the media.'),
      'cutline' => t('Cutline text for the media.'),
      'mediatype' => t('Type of media (Wide, Centrial, Inline, Associated)'),
    );
    
    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
    
    // Set up our destination - fieldcollection in this case.
    $this->destination = new MigrateDestinationFieldCollection('field_mediacollection', array('host_entity_type' => 'node'));
    
    // FIELD MAPPINGS
    $this->addFieldMapping('host_entity_id', 'nid')->sourceMigration('NodeArticle');
    $this->addFieldMapping('field_media', 'mediafile')->arguments(array('file_function'=>'file_link','preserve_files' => TRUE));
    $this->addFieldMapping('field_credit', 'credit');
    $this->addFieldMapping('field_cutline', 'cutline');
    $this->addFieldMapping('field_mediatype', 'mediatype')->defaultValue(0);
    $this->addFieldMapping('field_thumbnailselected')->defaultValue(0);
    $this->addFieldMapping('field_thumbnaildata')->defaultValue('');
    $this->addFieldMapping('field_credit_link')->defaultValue('');
    $this->addFieldMapping('field_cutline_link')->defaultValue('');
    
    // Unmapped source fields
    $this->addUnmigratedSources(array('filepath','fid','field_display_image_data','nid'));
    
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('path','field_crop_data'));
    
    // Disable application of pathauto during migration
    // $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
    $data = '';
    
    // wide image => mediatype main    
    $query = Database::getConnection('default', 'd6')->select('content_field_wide_image', 'c');
    $query->fields('c', array('field_wide_image_data'));
    $query->condition('c.field_wide_image_fid',$current_row->fid,'=');
    if ($data = $query->execute()->fetchField()) {
      $current_row->mediatype = 'main';
    } else {
      // inline image => mediatype vertical    
      $query = Database::getConnection('default', 'd6')->select('content_field_inline_image', 'c');
      $query->fields('c', array('field_inline_image_data'));
      $query->condition('c.field_inline_image_fid',$current_row->fid,'=');
      if ($data = $query->execute()->fetchField()) {
        $current_row->mediatype = 'vertical';
      } else {
        // central image => mediatype main    
        $query = Database::getConnection('default', 'd6')->select('content_type_content_article', 'c');
        $query->fields('c', array('field_central_image_data'));
        $query->condition('c.field_central_image_fid',$current_row->fid,'=');
        if ($data = $query->execute()->fetchField()) {
          $current_row->mediatype = 'main';
        } else {
          // associated image => mediatype none    
          $query = Database::getConnection('default', 'd6')->select('content_type_content_article', 'c');
          $query->fields('c', array('field_associated_image_data'));
          $query->condition('c.field_associated_image_fid',$current_row->fid,'=');
          $data = $query->execute()->fetchField();
        }
      }
    }

    // prepare content for field_media
    if ($uri = gs_migrate_mappedfileuri($current_row->fid)) {
      $image = array(
        'path' => $uri,
        'alt' => '',
        'title' => '',
      );
      $current_row->mediafile = drupal_json_encode($image);
      if ($image_data = gs_migrate_get_alt_title($data)) {
        $current_row->credit = text_summary($image_data['alt'],NULL,255);
        $current_row->cutline = text_summary($image_data['title'],NULL,255);
      }
    } else {
        return FALSE;
    }
  
    return TRUE;
  }
  
}