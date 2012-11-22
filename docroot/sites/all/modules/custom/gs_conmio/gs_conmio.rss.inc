<?php

/*************************************************
 * DRUPAL HOOKS
 *************************************************/

/**
 * Implements hook_views_pre_render()
 * 
 */
function gs_conmio_views_pre_render(&$view) {
  if ($view->name == 'conmio_feeds' && $view->current_display == 'conmio_feed_current_contests') {
    // This was written to address how Conmio currently does not support more complex contest fields.
    // To avoid any problems, we scan through each field in our contest and determine if it should
    // be excluded from the feed or not.
    foreach ($view->result as $key => $result) {
      $node = node_load($result->nid);
      
      // List of field types that are not supported on Conmio.
      $field_flags = array(
        'restaurant',
        'autocomplete',
        'file',
        'select',
        'date',
        'time',
        'textarea',
        'grid',
      );
      
      // Check each webform component and determine if the result should be excluded from the view.
      foreach ($node->webform['components'] as $cid => $component) {
        if (in_array($component['type'], $field_flags)) {
          unset($view->result[$key]);
        }
      }
    }
  }
}

/**
 * Implements hook_views_plugins
 */
function gs_conmio_views_plugins() {
  return array(   
    'access' => array(
      'conmio' => array(
        'title' => t('Conmio'),
        'help' => t('Access will be granted to conmio host addresses only.'),
        'handler' => 'views_plugin_access_conmio',
        'path' => drupal_get_path('module', 'gs_conmio') . '/views',
        'help topic' => 'access-conmio',
      ),
    ),
  );
}

/**
 * Implements hook_views_plugins_alter
 */
function gs_conmio_views_plugins_alter(&$plugins) {
  // Taxonomy view does not have a row plugin for RSS feeds. This is partly addressed
  // by creating a custom plugin that exposes the field results theme_preprocess_views_view_row_rss
  // where we handle the rest of the output to RSS.
  // TODO: It would be nice to turn this into a standalone plugin where each field can be configured.
  $plugins['row']['term_rss'] = array(
    'title' => t('Content'),
    'help' => t('Display the content with standard term view.'),
    'handler' => 'views_plugin_row_term_rss',
    'path' => drupal_get_path('module', 'gs_conmio') . '/views',
    'theme' => 'views_view_row_rss',
    'theme file' => 'theme.inc',
    'theme path' => drupal_get_path('module', 'views') . '/theme',
    'base' => array('taxonomy_term_data'),
    'uses fields' => TRUE,
    'uses options' => TRUE,
    'type' => 'feed',
  );
}

/*************************************************
 * PREPROCESSING
 *************************************************/

/**
 * Implements theme_preprocess_views_view_row_rss()
 * 
 */
function gs_conmio_preprocess_views_view_row_rss(&$vars) {
  $view = &$vars['view'];
  $row  = &$vars['row'];
  $elements = &$row->elements;
  $new_elements = array();
  
  if (in_array($view->name, array('conmio_feeds', 'conmio_feeds_comments', 'conmio_feeds_taxonomy'))) {
    if (isset($row->nid)) {
      $node = node_load($row->nid);
      if (isset($node->field_mediacollection)) {
         $image = _get_featured_media(_gs_helper_extract_select_values($node->field_mediacollection));
      }
    }
    elseif(isset($row->cid)) {
      $comment = comment_load($row->cid);
    }
    
    // The names for $view->current_display is defined in Advanced -> Other -> Machine Name
    switch ($view->current_display) {
      /******************************************************************************************
       List of Nodes View
       ******************************************************************************************/
      case 'conmio_feed_type_term':
        $image = _get_featured_media_data('75x75_mobile_thumb', $image);
        
        // Array of rss elements we want to add or modify.
        $new_elements = array(
          'guid' => $node->nid,
          'dc:image' => $image['image'],
        );
        break;
      /******************************************************************************************
       Single Node View
       ******************************************************************************************/
      case 'conmio_feed_node':
        $image = _get_featured_media_data('300w_mobile_image', $image);
        $body = field_get_items('node', $node, 'body');
        
        $vars['description'] = _convert_relative_links_to_absolute($body[0]['value']);
        
        // Array of rss elements we want to add or modify.
        $new_elements = array(
          'guid' => $node->nid,
          'comment_status' => $node->comment,
          'image_wide' => $image['image'],
          'publish_date' => date("F j, Y", _get_publish_date($node)),
          'sections' => _get_sections($node),
          'contributors' => _get_authors($node),
          'web_url' => $row->link,
        );
        if ($image['cutline']) {
          $new_elements['image_wide_caption'] = $image['cutline'];
        }
        if ($image['credit']) {
          $new_elements['image_wide_credit'] = $image['credit'];
        }
        if ($videos = _extract_youtube_links($body[0]['value'])) {
          $new_elements['youtube'] = $videos;
        }
        if ($sidebar = _get_legacy_sidebar($node)) {
          $new_elements = array_merge($new_elements, $sidebar);
        }
        break;
      /******************************************************************************************
       List of Comments for Node View
       ******************************************************************************************/
      case 'conmio_feed_node_comments':
        $body = field_get_items('comment', $comment, 'comment_body');
        $vars['description'] = _convert_relative_links_to_absolute($body[0]['value']);
        $vars['title'] = '';
        
        // Array of rss elements we want to add or modify.
        $new_elements = array(
          'guid' => $comment->cid,
          'uid' => $comment->uid,
          'name' => _get_commenter($comment),
          'timestamp' => $comment->created,
        );
        break;
      /******************************************************************************************
       List of Current Contests View
       ******************************************************************************************/
      case 'conmio_feed_current_contests':
        $image = _get_featured_media_data('75x75_mobile_thumb', $image);
        
        if ($teaser = field_get_items('node', $node, 'field_subheadline')) {
          $vars['description'] = _convert_relative_links_to_absolute($teaser[0]['value']);
        }
        elseif ($body = field_get_items('node', $node, 'body')) {
          $vars['description'] = _convert_relative_links_to_absolute($body[0]['value']);
        }
        
        $category = field_get_items('node', $node, 'field_contest_category');
        $category = taxonomy_term_load($category[0]['tid']);
        
        $new_elements = array(
          'category' => array(
            'value' => $category->name,
            'attributes' => array(
              'domain' => '',
            ),
          ),
          'enclosure' => array(
            'attributes' => array(
              'url' => $image['image'],
              'length' => $image['size'],
              'type' => $image['mime'],
            ),
          ),
          'multiformID' => $node->nid,
          'formField' => _get_webform_fields($node),
        );
        if ($contest_period = _get_contest_period($node)) {
          $new_elements = array_merge($new_elements, $contest_period);
        }
        break;
      /******************************************************************************************
       List of Terms and their Node Count
       ******************************************************************************************/
      case 'conmio_feed_classifieds_term_children':
        $vars['link'] = 'http://' . $_SERVER['HTTP_HOST'] . '/rss/conmio/feed/classifieds/' . $row->tid;
        
        $new_elements = array(
          'amount' => $row->node_taxonomy_index_nid,
        );
        break;
      /******************************************************************************************
       List all Classified Ads from Term
       ******************************************************************************************/
      case 'conmio_feed_classified_ads':
        $description = field_get_items('node', $node, 'field_keywords');
        $body = field_get_items('node', $node, 'body');
        
        $bodyHTML = filter_dom_load($body[0]['value']);
        if ($image = $bodyHTML->getElementsByTagName('img')->item(0)) {
          $image = 'http://' . $_SERVER['HTTP_HOST'] . $image->getAttribute('src');
        }
        
        $new_elements = array();
        
        if (isset($image) && valid_url($image)) {
          $vars['description'] = check_plain(trim($description[0]['value']));
          
          $new_elements['enclosure'] = array(
            'attributes' => array(
              'url' => $image,
              'length' => 0,
              'type' => 'image/jpeg',
            ),
          );
        }
        else {
          $vars['description'] = check_plain($body[0]['value']);
        }
        
        break;
    }
    
    /******************************************************************************************
     Format New Elements for RSS Feed
     ******************************************************************************************/
    foreach ($new_elements as $element_key => $element_value) {
      $is_array = is_array($element_value);
      
      if ($element_key == 'formField') {
        $elements = array_merge($elements, $element_value);
        continue;
      }
      
      // Check if element already exists.
      if ($result = _search_key_value('key', $element_key, $elements)) {
        $elements[$result]['key'] = $element_key;
        
        if ($is_array) {
          foreach ($element_value as $key => $value) {
            $elements[$result][$key] = $value;
          }
        }
        else {
          $elements[$result]['value'] = $element_value;
        }
      }
      elseif (isset($element_value)) {
        $add_element = array();
        $add_element['key'] = $element_key;
        
        if ($is_array) {
          foreach ($element_value as $key => $value) {
            $add_element[$key] = $value;
          }
        }
        else {
          $add_element['value'] = $element_value;
        }
        
        $elements[] = $add_element;
      }
    }
    
    // Render the elements again.
    $vars['item_elements'] = empty($row->elements) ? '' : format_xml_elements($row->elements);
    
    if ($vars['id'] == 1) {
      /*dsm(array(
        'row' => $row,
        'node' => $node,
        'vars' => $vars,
      ));*/
    }
  }
}

/***************************************************
 * HELPERS 
 ***************************************************/

/**
 * Searching an array for a key and value and returns the parent key.
 *
 * @param $search_key
 *     The key to search in.
 * @param $search_value
 *     The key value to search for.
 * @param $array
 *     The array you want to search.
 *
 * @return $key
 *    Returns the parent key value, and false if none is found.
 */
function _search_key_value($search_key, $search_value, $array) {
  if (!(is_array($array) && isset($search_key, $search_value))) {
    return FALSE;
  }
  
  foreach ($array as $key => $value) {
    if (isset($array[$key][$search_key]) && $array[$key][$search_key] == $search_value) {
      return $key;
    }
  }
  
  return FALSE;
}

/**
 * Get the featured media from the field_mediacollection.
 *
 * @param $media
 *     An array of field collection entity ids.
 *
 * @return Object
 *    Returns the field collection entity object.
 */
function _get_featured_media($media) {
  if (array_sum($media)) {
    $media = entity_load('field_collection_item', $media);
  }
  else {
    return NULL;
  }
  
  foreach ($media as $entity) {
    $media_type = isset($entity->field_mediatype[LANGUAGE_NONE]) ? $entity->field_mediatype[LANGUAGE_NONE][0]['value'] : NULL;
    
    // Main Image should take presedence over Vertical.
    if ($media_type == 'main') {
      return $entity;
    }
    elseif ($media_type == 'vertical') {
      $result = $entity;
    }
  }
  
  if (isset($result)) {
    return $result;
  }
  
  return NULL;
}

/**
 * Get the featured media data.
 *
 * @param $image_style
 *     The image style to use for the url.
 * @param $media_entity
 *     The field collection entity
 *
 * @return array
 *    Returns uri, type, image_url, cutline, and credit.
 */
function _get_featured_media_data($image_style, $media_entity) {
  if (!(is_object($media_entity) && image_style_load($image_style))) {
    return NULL;
  }
  
  $field_media     = field_get_items('field_collection_item', $media_entity, 'field_media');
  $field_mediatype = field_get_items('field_collection_item', $media_entity, 'field_mediatype');
  $field_cutline   = field_get_items('field_collection_item', $media_entity, 'field_cutline');
  $field_credit    = field_get_items('field_collection_item', $media_entity, 'field_credit');
  
  $media_type = isset($field_media[0]['type']) ? $field_media[0]['type'] : NULL;
  $media_mime = isset($field_media[0]['filemime']) ? $field_media[0]['filemime'] : NULL;
  $media_size = isset($field_media[0]['filesize']) ? $field_media[0]['filesize'] : NULL;
  
  switch ($media_type) {
    case 'image':
      $media_uri = $field_media[0]['uri'];
      break;
    case 'video':
      if (file_uri_scheme($field_media[0]['uri']) == 'youtube') {
        $media_uri = file_stream_wrapper_get_instance_by_uri($field_media[0]['uri'])->getLocalThumbnailPath();
      }
      break;
  }
  
  return array(
    'uri'     => $field_media[0]['uri'],
    'image'   => $media_uri ? image_style_url($image_style, $media_uri) : FALSE,
    'type'    => $media_type,
    'cutline' => $field_cutline ? $field_cutline[0]['value'] : NULL,
    'credit'  => $field_credit ? $field_credit[0]['value'] : NULL,
    'mime'    => $media_mime,
    'size'    => $media_size,
  );
}

/**
 * Get all terms for node.
 *
 * @param $node
 *     The node object.
 *
 * @return array
 *     A RSS ready array of section terms and name.
 */
function _get_sections($node) {
  $terms = $results = NULL;
  
  switch ($node->type) {
    case 'article':
      $terms = field_get_items('node', $node, 'field_section');
      $terms = array(taxonomy_term_load(end(end($terms))));
      
      if ($crosspost_topics = field_get_items('node', $node, 'field_secondarysections')) {
        foreach ($crosspost_topics as $crosspost_topic) {
          $terms[] = taxonomy_term_load($crosspost_topic['tid']);
        }
      }
      break;
    case 'blog_post':
      $terms = field_get_items('node', $node, 'field_blog_term');
      $terms = array(taxonomy_term_load(end(end($terms))));
      
      if ($crosspost_topics = field_get_items('node', $node, 'field_crosspostingtags')) {
        foreach ($crosspost_topics as $crosspost_topic) {
          $terms[] = taxonomy_term_load($crosspost_topic['tid']);
        }
      }
      break;
  }
  
  if (!empty($terms)) {
    $results = array();
    
    foreach ($terms as $term) {
      if (!is_object($term)) {
        continue;
      }
      
      $results[$term->tid] = array(
        'key' => 'section',
        'value' => array(
          0 => array(
            'key' => 'name',
            'value' => $term->name,
          ),
          1 => array(
            'key' => 'id',
            'value' => $term->tid,
          ),
        ),
      );
    }
  }
  
  return $results;
}

/**
 * Get the publish date of node. Uses schedule date if set, otherwise uses node created date.
 *
 * @param $node
 *     The node object.
 *
 * @return
 *     A UNIX timestamp.
 */
function _get_publish_date($node) {
  return isset($node->scheduler) ? $node->scheduler['publish_on'] : $node->created;
}

/**
 * Get the authors (contributors/bylines) of node.
 *
 * @param $node
 *     The node object.
 *
 * @return array
 *     A RSS ready array of authors.
 */
function _get_authors($node) {
  $authors = field_get_items('node', $node, 'field_byline');
  $results = NULL;
  
  if (!empty($authors)) {
    $results = array();
    
    foreach ($authors as $author) {
      $account = user_load($author['uid']);
      
      $results[$account->uid] = array(
        'key' => 'contributor',
        'value' => array(
          0 => array(
            'key' => 'name',
            'value' => $account->name,
          ),
          1 => array(
            'key' => 'uid',
            'value' => $account->uid,
          ),
        ),
      );
    }
  }
  
  return $results;
}

/**
 * Extract youtube videos from content.
 *
 * @param string $body
 *     The html body to parse.
 *
 * @return array
 *     A RSS ready array of youtube video ids.
 */
function _extract_youtube_links($body) {  
  if (preg_match_all('/(?:src=[\'|"]https?:\/\/)?(?:www\.)?(?:youtu\.be\/|youtube\.com(?:\/embed\/|\/v\/|\/watch\?v=))([A-Za-z0-9_-]{5,11})(?:[\'|"])/', $body, $matches)) {
    $youtube_link_elements = array();
    
    foreach($matches[1] as $youtube_id) {
      $youtube_link_elements[$youtube_id] = array(
        'key' => 'link',
        'value' => 'http://www.youtube.com/watch?v=' . $youtube_id,
      );
    }
    
    if (!empty($youtube_link_elements)) {
      return $youtube_link_elements;
    }
  }
  
  return NULL;
}

/**
 * Extract legacy sidebar.
 *
 * @param $node
 *     The node object.
 *
 * @return array
 *     A RSS ready array of sidebar title and body.
 */
function _get_legacy_sidebar($node) {
  $result = NULL;
  
  if ($legacy_sidebar = field_get_items('node', $node, 'field_legacy_sidebar')) {
    $result['sidebar_body'] = _convert_relative_links_to_absolute($legacy_sidebar[0]['value']);
    
    if ($legacy_sidebar_title = field_get_items('node', $node, 'field_legacy_sidebar_title')) {
      $result['sidebar_title'] = $legacy_sidebar_title[0]['value'];
    }  
  }
  
  return $result;
}

/**
 * Replace all relative links with absolute links.
 *
 * @param string $body
 *     The html body to parse.
 *
 * @return string
 *     A result string after replace.
 */
function _convert_relative_links_to_absolute($body) {
  $body = htmlspecialchars_decode($body);
  return check_plain(preg_replace_callback('/(href|src)=\"([^:\"]*)(?:\"|(?:(?:%20|\s|\+)[^\"]*\"))/', 'rel2abs', $body));
}
// Callback function for preg_replace_callback()
function rel2abs($matches){
  $protocol = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != "off") ? "https" : "http";
  $base_url = $protocol . '://' . $_SERVER['HTTP_HOST'] . '/' . $_SERVER['REQUEST_URI'];
  $relative_link = $matches[2];
  
  // Check if it's a relative link.
  if (parse_url($relative_link, PHP_URL_SCHEME) != '') {
    $url = $relative_link;
  }
  // Check if it's a query or anchor.
  elseif ($relative_link[0] == '#' || $relative_link[0] == '?') {
    $url = $base_url . $relative_link;
  }
  
  // Create variables for each component from parse_url()
  // $scheme, $host, $path
  extract(parse_url($base_url));
  
  $abs = ($relative_link[0] == '/' ? '' : preg_replace('#/[^/]*$#', '', $path))."/$relative_link";
  $re  = array('#(/\.?/)#', '#/(?!\.\.)[^/]+/\.\./#');
  
  // Recursively go through relative link and correctly format it as a absolute.
  for ($n = 1; $n > 0; $abs = preg_replace($re, '/', $abs, -1, $n));
  
  $url = $scheme . '://' . $host . str_replace('../', '', $abs);
  
  if (!empty($url)) {
    return $matches[1] . '="' . $url . '"';
  }
  
  return $matches[0];
}

/**
 * Get the name of commenter.
 *
 * @param $comment
 *     The comment object.
 *
 * @return string
 *     The name of commenter.
 */
function _get_commenter($comment) {
  $name = variable_get('anonymous', t('Anonymous'));
  
  if ($comment->uid && module_exists('profile2')) {
    if ($profile = profile2_load_by_user(user_load($comment->uid), 'profile')) {
      $field_first_name = field_get_items('profile2', $profile, 'field_first_name');
      $field_last_name = field_get_items('profile2', $profile, 'field_last_name');
      
      if (!empty($field_first_name[0]['value'])) {
        $name = $field_first_name[0]['value'];
      }
      if (!empty($field_last_name[0]['value'])) {
        $name .= ' ' . $field_last_name[0]['value'];
      }
    }
  }
  elseif ($field_yourname = field_get_items('comment', $comment, 'field_yourname')) {
    $name = $field_yourname[0]['value'];
  }
  
  return $name;
}

/**
 * Get contest period.
 *
 * @param $node
 *     The contest node object.
 *
 * @return array
 *     A RSS ready array of contest start and end date.
 */
function _get_contest_period($node) {
  $result = NULL;
  
  if ($contest_period = field_get_items('node', $node, 'field_contest_period')) {
    $result['activationDate'] = $contest_period[0]['value'];
    $result['expiryDate'] = $contest_period[0]['value2']; 
  }
  
  return $result;
}

/**
 * Get webform fields from contest.
 *
 * @param $node
 *     The contest node object.
 *
 * @return array
 *     A RSS ready array of contest fields.
 */
function _get_webform_fields($node) {
  $results = NULL;
  
  $field_to_ignore = array(
    'pagebreak',
    'markup',
    'consent',
  );
  
  foreach ($node->webform['components'] as $key => $component) {
    if (in_array($component['type'], $field_to_ignore)) {
      continue;
    }
    $results[] = array(
      'key' => 'formField',
      'value' => $component['type'] == 'skill_test' ? 'Answer Skill Testing Question' : $component['name'],
      'attributes' => array(
        'id' => 'multiforms_view[' . $component['cid'] . ']',
      ),
    );
  }
  
  return $results;
}