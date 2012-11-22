<?php

// TEST fids:
// 25147,25149,25150,25151,25152,25153,25154,25155,25156,25157,25158,25159,25160,25161,25162,25163,25164,25165,25166,25168,25169,25170,25171,25172,25173,25174,25175,25176,25177,25178,25179,25180,25181,25189,25190,25191,25194,25195,25196,25197,25198,25199,25200,25201,25202,25203,25204,25205,25206,25207,25208,25209,25210
// TEST tid: 218

/**
 * Migration class for mediacollection field for gallery nodes
 */
class NodeGalleryMediacollectionMigration extends GSMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate GS Galleries media collection field');
    $this->dependencies = array('NodeGallery');
    
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
    
    // find all term ids inside Gallery taxonomy
    $subquery = Database::getConnection('default', 'd6')->select('term_data', 'td');
    $subquery->fields('td', array('tid'));
    $subquery->condition('td.vid', 6, '='); // "Galleries" vocabulary id
    $subquery->condition('td.tid', array('196437'), 'NOT IN'); // exclude "Ad-Hoc Image" gallery
    $subquery->join('term_node','tn','tn.tid = td.tid');
    $subquery->groupBy('td.tid');
    $tids = $subquery->execute()->fetchCol();
    
    $query = Database::getConnection('default','d6')->select('term_node','tn');
    $query->fields('tn', array('tid'));
    $query->join('content_type_content_gallery_image','gn','tn.nid = gn.nid');
    $query->addField('gn', 'field_display_image_data');
    $query->addField('gn', 'field_photo_credit_value');
    $query->join('files','f','f.fid = gn.field_display_image_fid');
    $query->addField('f', 'filepath');
    $query->addField('f', 'fid');
    $query->condition('tn.tid',$tids,'IN');
    $query->groupBy('f.fid');
    
    // source fields
    $source_fields = array(
      'tid' => t('Term ID.'),
      'mediafile' => t('Multiple images from nodes tagged with this term.'),
      'filepath' => t('Path of the file.'),
      'fid' => t('Unique ID for the file.'),
      'field_display_image_data' => t('Serialized array with metadata of the file.'),
      'field_photo_credit_value' => t('Serialized array with metadata of the file.'),
      'credit' => t('Credit text for the media.'),
      'cutline' => t('Cutline text for the media.'),
    );
    
    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
    
    // Set up our destination - fieldcollection in this case.
    $this->destination = new MigrateDestinationFieldCollection('field_mediacollection', array('host_entity_type' => 'node'));
    
    // FIELD MAPPINGS
    $this->addFieldMapping('host_entity_id', 'tid')->sourceMigration('NodeGallery');
    $this->addFieldMapping('field_media', 'mediafile')->arguments(array('file_function'=>'file_link','preserve_files' => TRUE));
    $this->addFieldMapping('field_credit', 'credit')->defaultValue('');
    $this->addFieldMapping('field_cutline', 'cutline')->defaultValue('');
    $this->addFieldMapping('field_thumbnailselected')->defaultValue(0);
    $this->addFieldMapping('field_thumbnaildata')->defaultValue('');
    $this->addFieldMapping('field_credit_link')->defaultValue('');
    $this->addFieldMapping('field_cutline_link')->defaultValue('');
    $this->addFieldMapping('field_mediatype')->defaultValue(0);
    
    // Unmapped source fields
    $this->addUnmigratedSources(array('filepath','fid','field_display_image_data','field_photo_credit_value'));
    
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('path','field_crop_data'));
    
    // Disable application of pathauto during migration
    // $this->addFieldMapping('pathauto')->defaultValue(0);
  }
  
  public function prepareRow($current_row) {
    // prepare content for field_media
    if ($uri = gs_migrate_mappedfileuri($current_row->fid)) {
      $image_data = gs_migrate_get_alt_title($current_row->field_display_image_data);
      $image = array(
        'path' => $uri,
        'alt' => $image_data['alt'],
        'title' => $image_data['title'],
      );
      $current_row->credit = $current_row->field_photo_credit_value;
      $current_row->cutline = $image_data['title'];
      $current_row->mediafile = drupal_json_encode($image);
    } else {
        return FALSE;
    }
  
    return TRUE;
  }
  
}