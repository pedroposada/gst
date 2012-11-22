<?php
/**
 * @file
 * Contains code specific to Display Suite's theming functionality.
 */

/**
 * Content for DS author_date field.
 */
function theme_gs_helper_author_date($vars) {
  $output = '';
  
  if (isset($vars['entity'])) {
    $byline = '';
    if (isset($vars['entity']->field_byline['und'][0])) {
      $byline = array();
      foreach ($vars['entity']->field_byline['und'] as $item) {
        $account = user_load($item['uid']);
        $byline[] = '<span class="name">' . theme('username', array('account' => $account)) . '</span>';
      }
      $byline = array_unique($byline);
    }
    if (!empty($byline)) {
      $byline = t('by !byline on', array('!byline' => implode(', ', $byline)));
    }
    $timestamp = isset($vars['entity']->scheduler) ? $vars['entity']->scheduler['publish_on'] : $vars['entity']->created;
    $date = '<span class="date">' . format_date($timestamp, 'normal_short') . '</span>';
    $time = '<span class="time">' . format_date($timestamp, 'time_normal') . '</span>';
    switch ($vars['entity_type']) {
      case 'node':
        $output = t('!byline !date at !time', array('!byline' => $byline, '!date' => $date, '!time' => $time));
        break;
        
      case 'comment':
        $date_time = '<span class="date-time">' . t('!date at !time', array('!date' => $date, '!time' => $time)) . '</span>';
        $name = '';
        if ($account = user_load($vars['entity']->uid)) {
          $name = theme('username', array('account' => $account));
        }
        $output = t('!name !date_time', array('!name' => $name, '!date_time' => $date_time));
        break;
    }
  }
  
  return $output;
}

/**
 * Comment date format for comment post date.
 */
function theme_gs_helper_comment_author($vars) {
  $output = '';
 
  if (isset($vars['entity'])) {
    switch ($vars['entity_type']) {
      case 'comment':
      if ( $vars['entity']->uid ) {
        $account = user_load($vars['entity']->uid);
        $name = theme('username', array('account' => $account));
      } 
      elseif ( isset($vars['entity']->field_yourname) && !empty($vars['entity']->field_yourname['und'][0]['value']) ) {
        $name = $vars['entity']->field_yourname['und'][0]['value'];
      } 
      else {
        $name = variable_get('anonymous', t('Anonymous'));
      }
      $output = '<div class="comment-author">' . t('!name', array('!name' => $name)) . '</div>';
      break;
    }
  }
  
  return $output;
}

/**
 * Comment rate widget.
 */
function theme_gs_helper_comment_rate_widget($vars) {
  $output = '';
 
  if (isset($vars['entity'])) {
    switch ($vars['entity_type']) {
      case 'comment':
        $id = $vars['entity']->cid;
        $type = substr($vars['entity']->node_type, strlen('comment_node_'));
        break;
      case 'node':
        $id = $vars['entity']->nid;
        $type = $vars['entity']->type;
        break;
    }
    $widgets = rate_get_active_widgets($vars['entity_type'], $type);
    foreach ((array) $widgets as $widget_id => $widget) {      
      $output .= '<div class="comment-rate-widget">'; 
      $output .= rate_generate_widget($widget_id, $vars['entity_type'], $id);
      $output .= '</div>';
    }
  }

  return $output;
}

/**
 * Comment date format for comment post date.
 */
function theme_gs_helper_comment_post_date($vars) {
  $output = '';
  
  if (isset($vars['entity'])) {
    
    $date = '<span class="date">' . format_date($vars['entity']->created, 'normal_short') . '</span>';
    $time = '<span class="time">' . format_date($vars['entity']->created, 'time_normal') . '</span>';
    
    switch ($vars['entity_type']) {
        
      case 'comment':
        $date_time = '<span class="date-time">' . t('!date at !time', array('!date' => $date, '!time' => $time)) . '</span>';
        
        $output = t('!date_time', array('!date_time' => $date_time));
        
        break;
    }
  }
  
  return $output;
}

/**
 * Content for DS gs_helper_featured_media field.
 */
function theme_gs_helper_featured_media($vars) {
  $output = $content = $credit = $cutline = $featured = $ids = NULL;
  $width = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['width']) ? 
  $vars['formatter_settings']['width'] : 600;
  $height = $width*480/640;
  $main = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['wide_image_style_name']) ? 
  $vars['formatter_settings']['wide_image_style_name'] : 'article_main';
  // $file_type = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['file_type']) ? 
  // $vars['formatter_settings']['file_type'] : 'any';
  
  if (isset($vars['entity'])) {
    // check if there are images
    if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
      // extract vertical or main media
      $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
      $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
      foreach ($fieldcollection as $entityId => $entity) {
        $media_type = isset($entity->field_mediatype['und']) ? $entity->field_mediatype['und'][0]['value'] : NULL;
        // main has presedence
        if ($media_type == 'main') {
          $featured = $entity;
          $style = $main;
          break;
        }
        // vertical
        if ($media_type == 'vertical') {
          $featured = $entity;
          $style = 'article_vertical';
        }
      }
      // output image
      if ($featured && $featured->field_media['und'][0]['type'] == 'image') {
        $variables = array(
          'style_name' => $style,
          'path' => $featured->field_media['und'][0]['uri'],
          'title' => (isset($featured->field_cutline['und']) ? $featured->field_cutline['und'][0]['value'] : NULL),
          //'alt' => (isset($featured->field_credit['und']) ? $featured->field_credit['und'][0]['value'] : NULL),
        );
        $content = theme('image_style',$variables);
      }
      // output video
      if ($featured && $featured->field_media['und'][0]['type'] == 'video') {
        switch ($featured->field_media['und'][0]['filemime']) {
            case 'video/youtube':
              $video = theme('media_youtube_video', array('uri' => $featured->field_media['und'][0]['uri'], 'width' => $width, 'height' => $height));
              break;
            case 'video/vimeo':
              $wrapper = file_stream_wrapper_get_instance_by_uri($featured->field_media['und'][0]['uri']);
              $parts = $wrapper->get_parameters();
              $video_id = check_plain($parts['v']);
              $video = '<iframe src="http://player.vimeo.com/video/' . $video_id . '?byline=0&amp;portrait=0&amp;color=ee3a43" width="' . $width . '" height="' . $height . '" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe>';
              break;
            default:
              $video_vars = array(
                'items'       => array(
                  'movie' => $featured->field_media['und'][0],
                ),
                'attributes'  => array(
                  'width' => $width,
                  'height' => $height,
                ),
                'player_id'   => 'file-' . $featured->field_media['und'][0]['fid'],
              );
              $video = theme('videojs', $video_vars);
              break;
        }
        $content = $video;
      }
      // content wrapper
      if ($content) {
        if (isset($featured->field_cutline['und']) && !empty($featured->field_cutline['und'][0]['value'])) {
          $cutline  = '<div class="featured-media-cutline">';
          $cutline .= (isset($featured->field_cutline_link['und']) && strlen($featured->field_cutline_link['und'][0]['value']) ? 
            l($featured->field_cutline['und'][0]['value'],$featured->field_cutline_link['und'][0]['value'],array('html'=>TRUE)) : 
            $featured->field_cutline['und'][0]['value']);
          $cutline .= '</div>';
        }
        if (isset($featured->field_credit['und']) && !empty($featured->field_credit['und'][0]['value'])) {
          $credit  = '<div class="featured-media-credit">';
          $credit .= (isset($featured->field_credit_link['und']) && strlen($featured->field_credit_link['und'][0]['value']) ? 
            l($featured->field_credit['und'][0]['value'],$featured->field_credit_link['und'][0]['value'],array('html'=>TRUE)) : 
            $featured->field_credit['und'][0]['value']);
          $credit .= '</div>';
        }
        $output .= '<div class="article-featured-media ' . $style . '">';
        $output .= $content;
        if ($vars['view_mode'] != 'print') {
          $output .= $cutline;
          $output .= $credit;
        }
        $output .= '</div>';
      }
    }
  }
  
  return $output;
}
/**
 * Content for DS gs_helper_body_first_paragraph field.
 */
function theme_gs_helper_body_first_paragraph($vars) {
  $output = '';
  
  if (isset($vars['entity']) && !empty($vars['entity']->body['und'][0]['safe_value'])) {
    $paragraphs = explode('</p>', $vars['entity']->body['und'][0]['safe_value']);
    $first = array_shift($paragraphs) . '</p>';
    $output = $first;
  }
  
  return $output;
}

/**
 * Content for DS gs_helper_body_second_paragraph field.
 */
function theme_gs_helper_body_second_paragraph($vars) {
  $output = '';
  
  if (isset($vars['entity']) && !empty($vars['entity']->body['und'][0]['value'])) {

    // display text
    if ($disableFirstParagraphField) {
      $output .= $vars['entity']->body['und'][0]['value'];
    }
    else {
        $paragraphs = explode('</p>', $vars['entity']->body['und'][0]['value']);
        array_shift($paragraphs);
        $output .= implode('</p>', $paragraphs);
    }

  }
  
  return $output;
}

/**
 * Content for DS gs_helper_sidebar_node field.
 */
function theme_gs_helper_sidebar_node($vars) {
  $output = '';
  
  $reaction = context_get_plugin('reaction', 'block');
  $blocks = $reaction->block_get_blocks_by_region('sidebar_node');
  
  foreach ($blocks as &$block) {
    if (isset($block['#block']->region)) {
      // we have to 'disable' the region property of all blocks, otherwise no
      // content displays on the page at all
      $block['#block']->region = 'none';
      
      // we must have the content property set, or block.module will throw errors
      if (!isset($block['#block']->content)) {
        $block['#block']->content = '';
      }
    }
  }
  
  $output = render($blocks);
  
  return $output;
}

/**
 * Videos tab
 * Content for DS gs_helper_embedded_videos field.
 */
function theme_gs_helper_embedded_videos($vars) {
  $output = $video_ids = $content = $video = NULL;
  $width = isset($vars['width']) ? $vars['width'] : 600;

  // grab videos from node media
  if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
    $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
    $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
    foreach ($fieldcollection as $key => $entity) {
      $file = (isset($entity->field_media['und']) ? $entity->field_media['und'][0] : NULL);
      if ($file && $file['type'] == 'video') {
        $video = NULL;
        switch ($file['filemime']) {
          case 'video/youtube':
            $wrapper = file_stream_wrapper_get_instance_by_uri($file['uri']);
            $parts = $wrapper->get_parameters();
            $video_id = check_plain($parts['v']);
            if (isset($vars['entity']->body['und']) && !strstr($vars['entity']->body['und'][0]['value'], $video_id)) {
              $video = theme('media_youtube_video', array('uri'=>$file['uri'],'width'=>$width));
            }
            break;
          case 'video/vimeo':
            $wrapper = file_stream_wrapper_get_instance_by_uri($file['uri']);
            $parts = $wrapper->get_parameters();
            $video_id = check_plain($parts['v']);
            if (isset($vars['entity']->body['und']) && !strstr($vars['entity']->body['und'][0]['value'], $video_id)) {
              $video = theme('media_vimeo_video', array('uri'=>$file['uri'], 'width'=>$width));
            }
            break;
          default:
            $video_vars = array(
              'items'       => array(
                'movie' => $file,
              ),
              'attributes'  => array(
                'width' => $width,
                // 'height' => 300,
              ),
              'player_id'   => 'file-' . $file['fid'],
            );
            $video = theme('videojs', $video_vars) . '</div>';
            break;
        }
        ($video ? $content .= '<div class="video">' . $video . '</div>' : NULL);
      }
    }
  }
  // embedded videos
  if (isset($vars['entity']) && isset($vars['entity']->embedded_videos)) {
    foreach ($vars['entity']->embedded_videos as $iframe) {
      $content .= '<div class="video">' . $iframe . '</div>';
    }
  }
  // output
  if ($content) {
    // add galleria plugin 
    if ($path = libraries_get_path('galleria')) {
      drupal_add_js($path . '/galleria-1.2.7.js',array('scope'=>'header'));
      drupal_add_js($path . '/themes/twelve/galleria.twelve.js',array('scope'=>'footer'));
      drupal_add_css($path . '/themes/twelve/galleria.twelve.css',array('scope'=>'footer'));
    }
    $output .= '<div class="videos_tab_items_list">';
    $output .= $content;  
    $output .= '</div>';
  }
  
  return $output;
}

/**
* Content for DS gs_helper_section_topic field.
*/
function theme_gs_helper_section_topic($vars) {
  $output = '';

  if (isset($vars['entity']) && !empty($vars['entity']->field_section['und'][0])) {
    $size = sizeof($vars['entity']->field_section['und']);
    
    $last = taxonomy_term_load($vars['entity']->field_section['und'][$size - 1]['tid']);

    $output .= l($last->name, 'taxonomy/term/' . $last->tid);
  }
  
  return $output;
}

/**
 * Photos tab
 */
function theme_gs_helper_photos_tab($vars) {
  $output = $content = '';
  
  if (isset($vars['entity'])) {
    // grab image from node if any
    $image_vars = array(
      'style_name' => 'slideshow',
    );
    if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
      $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
      $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
      foreach ($fieldcollection as $key => $entity) {
        $file = (isset($entity->field_media['und']) ? $entity->field_media['und'][0] : NULL);
        if ($file && $file['type'] == 'image') {
          $image_vars['path'] = $file['uri'];
          $image_vars['alt'] = isset($entity->field_credit['und']) ? $entity->field_credit['und'][0]['value'] : '';
          $image_vars['title'] = isset($entity->field_cutline['und']) ? $entity->field_cutline['und'][0]['value'] : '';
          $content .= theme('image_style',$image_vars);
        }
      }
    }
    // grab images from attached gallery if any
    if (isset($vars['entity']->field_gallery['und'])) {
      $gallery = node_load($vars['entity']->field_gallery['und'][0]['nid']);
      if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
        $ids = _gs_helper_extract_select_values($gallery->field_mediacollection);
        $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
        foreach ($fieldcollection as $key => $entity) {
          $file = (isset($entity->field_media['und']) ? $entity->field_media['und'][0] : NULL);
          if ($file && $file['type'] == 'image') {
            $image_vars['path'] = $file['uri'];
            $image_vars['alt'] = isset($entity->field_credit['und']) ? $entity->field_credit['und'][0]['value'] : '';
            $image_vars['title'] = isset($entity->field_cutline['und']) ? $entity->field_cutline['und'][0]['value'] : '';
            $content .= theme('image_style',$image_vars);
          }
        }
      }
    }
  }
  if ($content) {
    // add galleria plugin 
    if ($path = libraries_get_path('galleria')) {
      drupal_add_js($path . '/galleria-1.2.7.js',array('scope'=>'header'));
      drupal_add_js($path . '/themes/twelve/galleria.twelve.js',array('scope'=>'footer'));
      drupal_add_css($path . '/themes/twelve/galleria.twelve.css',array('scope'=>'footer'));
    }
    $output .= '<div class="photos_tab_images_list">';
    $output .= $content;
    $output .= '</div>';
  }
  
  return $output;
}

/**
 * Downloads tab
 */
function theme_gs_helper_downloads_tab($vars) {
  $output = $content = '';
  
  if (isset($vars['entity'])) {
    // grab image from node if any
    if (isset($vars['entity']->field_files['und'])) {
      foreach ($vars['entity']->field_files['und'] as $key => $val) {
        $ext = pathinfo($val['filename'], PATHINFO_EXTENSION);
        $title = isset($val['description']) && strlen($val['description']) ? $val['description'] : $val['filename'];  
        $content .= "<div class=\"downloads-item $ext\">";
        $content .= "<p class=\"title\">$title</p>";
        $content .= "<p>".strtoupper($ext)." (".format_size($val['filesize']).")</p>";
        $content .= l(t('Download'), file_create_url($val['uri']));
        $content .= '</div>';
      }
    }
  }
  
  if ($content) {
    $output .= '<div class="downloads_tab_items_list">';
    $output .= $content;
    $output .= '</div>';
  }
  
  return $output;
}

/**
 * Prev and Next arrows for blog_posts
 */
function theme_gs_helper_prevnext($vars) {
    
  $output = $nids = NULL;
  
  if (isset($vars['entity'])) {
    if ($vars['entity']->type == 'blog_post' && isset($vars['entity']->field_blog_term['und'])) {
      // set defaults
      $prev_text = isset($vars['prev_text']) ? $vars['prev_text'] : "&laquo; Previous"; 
      $next_text = isset($vars['next_text']) ? $vars['next_text'] : "Next &raquo;"; 
      $prev = $next = $vars['entity']->nid;
      // find deepest tid from node
      $tid = $vars['entity']->field_blog_term['und'][0]['tid'];
      if (count($vars['entity']->field_blog_term['und'])>1) {
        $tids = _gs_helper_extract_select_values($vars['entity']->field_blog_term);
        $tid = array_pop($tids);
      }
      try {
        // find all nids in database      
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'node')
          ->entityCondition('bundle', 'blog_post')
          ->propertyCondition('status', 1)
          ->fieldCondition('field_blog_term', 'tid', $tid)
          ->propertyOrderBy('created', 'DESC');
        $result = $query->execute();
        $nids = array_keys($result['node']);
      } catch(Exception $e) {
        watchdog_exception('gs_helper', $e);
      }
      
      // output prev and next arrows
      if (count($nids)>1) {
        // find next and prev nid
        $key = array_search($vars['entity']->nid, $nids);
        $prev = isset($nids[$key - 1]) ? $nids[$key - 1] : $nids[count($nids) - 1];
        $next = isset($nids[$key + 1]) ? $nids[$key + 1] : $nids[0];
        // item list variables
        $variables = array(
          'items'=>array(
            array(
              'data' => l($prev_text,drupal_get_path_alias("node/$prev"),array(
                'html'=>TRUE,
                'attributes' => array(
                  'alt'=>t("Previous blog entry"),
                  'title'=>t("Previous blog entry"),
                ),
              )), 
              'class' => array('prev','prevnext')
            ),
            array(
              'data' => l($next_text,drupal_get_path_alias("node/$next"),array(
                'html'=>TRUE,
                'attributes' => array(
                  'alt'=>t("Next blog entry"),
                  'title'=>t("Next blog entry"),
                ),
              )), 
              'class' => array('next','prevnext')
            ),
          ),
          'attributes' => array(
            'class'=>array('prevnext-list'),
          ),
        );
        $output .= theme('item_list',$variables);
      }
    }
  }
  
  return $output;
}
/**
 * tags for blog pages
 */
function theme_gs_helper_blog_tags($vars) {
  $output = NULL;
  
  if (isset($vars['entity'])) {
    if ($vars['entity']->type == 'blog_post') {
      $items = array();
      
      // Get the Main Blog Topic
      if ($main_topics = field_get_items('node', $vars['entity'], 'field_blog_term')) {
        foreach ($main_topics as $topic) {
          $term = taxonomy_term_load($topic['tid']);
          if (!is_object($term)) {
            continue;
          }
          $items[$term->tid] = array(
            'data' => l($term->name, drupal_get_path_alias('taxonomy/term/' . $term->tid)),
            'class' => array('tag'),
            'id' => "tag-{$term->tid}",
          );
        }
      }
      
      // Get the Cross Posted Topics
      if (isset($vars['entity']->field_crosspostingtags['und'])) {
        foreach ($vars['entity']->field_crosspostingtags['und'] as $key => $val) {
          $term = taxonomy_term_load($val['tid']);
          if (!is_object($term)) {
            continue;
          }
          $items[$term->tid] = array(
            'data' => l($term->name, drupal_get_path_alias('taxonomy/term/' . $term->tid)),
            'class' => array('tag'),
            'id' => "tag-{$term->tid}",
          );
        }
      }
      
      $variables = array(
        'type' => 'ul',
        'items' => $items,
        'attributes' => array('class'=>array('tags-list')),
      );
      $output .= theme('item_list', $variables); 
    }
  }
  
  return $output;
}
/**
 * image thumbnail for nodes
 */
function theme_gs_helper_node_thumbnail($vars) {
  $output = NULL;
  
  $output = _gs_helper_node_thumbnail($vars);
  if (empty($output)) {
    // grab images from attached gallery if any
    if (isset($vars['entity']) && isset($vars['entity']->field_gallery['und'])) {
      $gallery = node_load($vars['entity']->field_gallery['und'][0]['nid']);
      if (isset($gallery->field_mediacollection['und'])) {
        $vars['entity'] = $gallery;
        $output = _gs_helper_node_thumbnail($vars);
      }
    }
  }
  
  return $output;
}
function _gs_helper_node_thumbnail($vars) {
  $output = $thumb = $t = $cropped = NULL;
  $styles = image_styles();
  $style = isset($vars['image_style_name']) && isset($styles[$vars['image_style_name']]) ? $vars['image_style_name'] : 'article_thumbnail';
  
  if (isset($vars['entity'])) {
    // check if there are images
    if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
      // extract images
      $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
      $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
      foreach ($fieldcollection as $entityId => $entity) {
        if (isset($entity->field_media['und']) && $entity->field_media['und'][0]['type'] == 'image') {
          $media_type = isset($entity->field_mediatype['und']) ? $entity->field_mediatype['und'][0]['value'] : NULL;
          if ($entity->field_thumbnailselected['und'][0]['value']) {
            $thumb['cropped'][] = $entity;
          }
          if ($media_type == 'main') {
            $thumb['main'][] = $entity;
          }
          if ($media_type == 'vertical') {
            $thumb['vertical'][] = $entity;
          }
          if (empty($media_type) && !$entity->field_thumbnailselected['und'][0]['value']) {
            $thumb['regular'][] = $entity;
          }
        }
      }
      // output image
      if (isset($thumb['cropped'])) {
        $output = $thumb['cropped'][0]->field_thumbnaildata['und'][0]['value'];  
      } elseif (isset($thumb['main'])) {
        $t = $thumb['main'][0];
      } elseif (isset($thumb['vertical'])) {
        $t = $thumb['vertical'][0];
      } else {
        $t = $thumb['regular'][0];
      }
      if ($t) {
        $variables = array(
          'style_name' => $style,
          'path' => $t->field_media['und'][0]['uri'],
          'alt' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
          'title' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
        );
        $output = theme('image_style',$variables);
      }
    }
  }
  
  return $output;
}

/**
 * universal thumbnail for nodes
 * output a thumbnail from a node
 * this will find the proper media item, video or image from mediacollection field
 */
function theme_gs_helper_node_thumbnail_universal($vars) {
  $output = NULL;
  $vars['image_style'] = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['image_style']) ? 
    $vars['formatter_settings']['image_style'] : NULL;
  $vars['link_content'] = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['link_content']) ? 
    $vars['formatter_settings']['link_content'] : NULL;
  $vars['display_cropped'] = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['display_cropped']) ? 
    $vars['formatter_settings']['display_cropped'] : NULL;
  $vars['file_type'] = isset($vars['formatter_settings']) && isset($vars['formatter_settings']['file_type']) ? 
    $vars['formatter_settings']['file_type'] : NULL;
  
  $output = _gs_helper_node_thumbnail_universal($vars);
  if (empty($output)) {
    // grab images from attached gallery if any
    if (isset($vars['entity']) && isset($vars['entity']->field_gallery['und'])) {
      $gallery = node_load($vars['entity']->field_gallery['und'][0]['nid']);
      if (isset($gallery->field_mediacollection['und'])) {
        $vars['entity'] = $gallery;
        $output = _gs_helper_node_thumbnail_universal($vars);
      }
    }
  }
  
  return $output;
}
function _gs_helper_node_thumbnail_universal($vars) {
  $output = $thumb = $t = $cropped = NULL;
  $styles = image_styles();
  $style = isset($vars['image_style']) && isset($styles[$vars['image_style']]) ? $vars['image_style'] : 'thumbnail';
  $link_content = isset($vars['link_content']) ? $vars['link_content'] : TRUE;
  $display_cropped = isset($vars['display_cropped']) ? $vars['display_cropped'] : TRUE;
  $file_type = isset($vars['file_type']) ? $vars['file_type'] : 'any';
  
  if (isset($vars['entity'])) {
    // check if there are images
    if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
      // extract images
      $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
      $fieldcollection = array_sum($ids) ? entity_load('field_collection_item', $ids) : array();
      foreach ($fieldcollection as $entityId => $entity) {
        if (isset($entity->field_media['und'])) {
          $media_type = isset($entity->field_mediatype['und']) ? $entity->field_mediatype['und'][0]['value'] : NULL;
          switch ($file_type) {
            case 'video':
              if ($entity->field_media['und'][0]['type'] == 'video') {
                if ($media_type == 'main') {
                  $thumb['main'][] = $entity;
                }
                if (empty($media_type) && !$entity->field_thumbnailselected['und'][0]['value']) {
                  $thumb['regular'][] = $entity;
                }
              }
              break;
            
            default:
              if ($entity->field_thumbnailselected['und'][0]['value']) {
                $thumb['cropped'][] = $entity;
              }
              if ($media_type == 'main') {
                $thumb['main'][] = $entity;
              }
              if ($media_type == 'vertical') {
                $thumb['vertical'][] = $entity;
              }
              if (empty($media_type) && !$entity->field_thumbnailselected['und'][0]['value']) {
                $thumb['regular'][] = $entity;
              }
              break;
          }
        }
      }
      // output image
      if (isset($thumb['cropped'])) {
        if ($display_cropped) {
          // Check if field_crop_data is set.
          if (isset($thumb['cropped'][0]->field_crop_data[LANGUAGE_NONE][0]['value'])) {
            $crop_data = unserialize($thumb['cropped'][0]->field_crop_data[LANGUAGE_NONE][0]['value']);
            // If there is no uri set. Output field_thumbnaildata instead.
            if (empty($crop_data['uri'][$style])) {
              $output = $thumb['cropped'][0]->field_thumbnaildata['und'][0]['value'];
            } else {
              $variables = array(
                'style_name' => $style,
                'alt' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
                'title' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
                'path' => $crop_data['uri'][$style],
              );
              $output = theme('image_style', $variables);
            }
          } else {
            $output = $thumb['cropped'][0]->field_thumbnaildata['und'][0]['value'];
          }
        } else {
          $t = $thumb['cropped'][0];
        }
      } elseif (isset($thumb['main'])) {
        $t = $thumb['main'][0];
      } elseif (isset($thumb['vertical'])) {
        $t = $thumb['vertical'][0];
      } else {
        $t = $thumb['regular'][0];
      }
      if ($t) {
        module_load_include('inc', 'gs_helper', 'gs_helper.images');
        
        $variables = array(
          'style_name' => $style,
          'alt' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
          'title' => (isset($t->field_cutline['und']) ? $t->field_cutline['und'][0]['value'] : ''),
        );
        // image thumbnail
        if ($crop_data = gs_helper_focus_crop_image($t, $style)) {
          $variables['path'] = $crop_data['uri'][$style];
        }
        elseif ($t->field_media['und'][0]['type'] == 'image') {
          $variables['path'] = $t->field_media['und'][0]['uri'];
        }
        // video thumbnail
        if ($t->field_media['und'][0]['type'] == 'video') {
          $scheme = file_uri_scheme($t->field_media['und'][0]['uri']);
          if (in_array($scheme, array('youtube', 'vimeo'))) {
            $wrapper = file_stream_wrapper_get_instance_by_uri($t->field_media['und'][0]['uri']);
            $variables['path'] = $wrapper->getLocalThumbnailPath();
          }
        }
        $html = isset($variables['path']) ? theme('image_style', $variables) : NULL;
        $output = $link_content && $html ? l($html, "node/{$vars['entity']->nid}",array('html'=>TRUE)) : $html;
      }
    }
  }
  return $output;
}
/**
 * Content for Display Suite gs_helper_media_gallery field.
 */
function theme_gs_helper_media_gallery($vars) {
  $output = '';
  $images = $ids = array();
  $media_type = 0;
  
  // add galleria plugin 
  if ($path = libraries_get_path('galleria')) {
    drupal_add_js($path . '/galleria-1.2.7.js',array('scope'=>'header'));
    drupal_add_js($path . '/themes/twelve/galleria.twelve.js',array('scope'=>'footer'));
    drupal_add_css($path . '/themes/twelve/galleria.twelve.css',array('scope'=>'footer'));
  }
  
  if (isset($vars['entity'])) {
    $node = $vars['entity'];
    $show_gallery = 0;
    
    if ($show_gallery = field_get_items('node', $node, 'field_gallery_show')) {
      $show_gallery = $show_gallery[0]['value'];
    }
    
    // Check if media is set to show as a gallery.
    if ($show_gallery) {
      // Get all the media in mediacollection.
      if ($mediacollection = field_get_items('node', $node, 'field_mediacollection')) {
        foreach ($mediacollection as $media) {
          if (is_numeric($media['value'])) {
            $ids[] = $media['value'];
          }
        }
      }
    }
    
    // Get all the media in the attached gallery.
    if ($gallery_id = field_get_items('node', $node, 'field_gallery')) {
      $gallery_node = node_load($gallery_id[0]['nid']);
      
      // Get all the media in mediacollection.
      if ($mediacollection = field_get_items('node', $gallery_node, 'field_mediacollection')) {
        foreach ($mediacollection as $media) {
          if (is_numeric($media['value'])) {
            $ids[] = $media['value'];
          }
        }
      }
    }
    
    // make sure all ids are greater than 0
    if (array_sum($ids)) {
      $mediacollection_items = entity_load('field_collection_item', $ids);
      foreach ($mediacollection_items as $entity) {
        $media_type = field_get_items('field_collection_item', $entity, 'field_mediatype');
        $media_type = $media_type[0]['value'];
        
        // Check if the media is featured (set as main or vertical).
        if (!in_array($media_type, array('main', 'vertical'))) {
          // Get all the image elements we need.
          $image = field_get_items('field_collection_item', $entity, 'field_media');
          $image_credit = field_get_items('field_collection_item', $entity, 'field_credit');
          $image_credit_link = field_get_items('field_collection_item', $entity, 'field_credit_link');
          $image_cutline = field_get_items('field_collection_item', $entity, 'field_cutline');
          $image_cutline_link = field_get_items('field_collection_item', $entity, 'field_cutline_link');
          if ($image[0]['type'] == 'image') {
            // Assign all the variables we need for theme().
            $images[$image[0]['fid']] = array(
              'style_name' => 'blog_main',
              'path' => $image[0]['uri'],
              'title' => $image_cutline ? $image_cutline[0]['value'] : '',
            );
            // Store all the descriptive elements seperately.
            $descriptions[$image[0]['fid']] = array(
              'credit' => $image_credit ? $image_credit[0]['value'] : '',
              'credit_link' => $image_credit_link ? $image_credit_link[0]['value'] : '',
              'cutline' => $image_cutline ? $image_cutline[0]['value'] : '',
              'cutline_link' => $image_cutline_link ? $image_cutline_link[0]['value'] : '',
            );
          }
        }
      }
      
      // Initialize the pager
      $total = count($images);
      $imgs_per_page = 10;
      $page = pager_default_initialize($total, $imgs_per_page);
      $offset = $imgs_per_page * $page;
      $images = array_slice($images, $offset, $imgs_per_page, true);
      
      foreach ($images as $key => $image) {
        $output .= '<div class="blog-gallery-img">';
        $output .= '<a href="' . image_style_url('popup', $image['path']) . '">' . theme('image_style', $image) . '</a>';
        $output .= !empty($descriptions[$key]['cutline']) ? '<span class="cutline">' . (valid_url($descriptions[$key]['cutline_link']) ? '<a href="' . $descriptions[$key]['cutline_link'] . '" target="_blank">' . $descriptions[$key]['cutline'] . '</a>' : $descriptions[$key]['cutline']) . '</span>' : '';
        $output .= !empty($descriptions[$key]['credit']) ? '<span class="credit">' . (valid_url($descriptions[$key]['credit_link']) ? '<a href="' . $descriptions[$key]['credit_link'] . '" target="_blank">' . $descriptions[$key]['credit'] . '</a>' : $descriptions[$key]['credit']) . '</span>' : '';
        $output .= '</div>';
      }
      
      $output .= '<div id="galleria"></div>';
    }
    
    $output .= theme('pager');
  }
  
  return $output;
}

/**
 * Content for Display Suite gs_helper_sidebar_blog field.
 */
/*
// Commented out because template.php already handles the output and it's styled.
// This can be reimplemented if flexibility is desired.
function theme_gs_helper_sidebar_blog($vars) {
  $output = '';
  
  if ($sidebar_content = field_get_items('node', $vars['entity'], 'field_legacy_sidebar')) {        
    $output = '<blockquote>';
    $output .= ($sidebar_title = field_get_items('node', $vars['entity'], 'field_legacy_sidebar_title')) ? '<h3 class="title">' . $sidebar_title[0]['value'] . '</h3>' : '';
    $output .= preg_replace('/<p>\s*<\/p>/', '', $sidebar_content[0]['value']);
    $output .= '</blockquote>';
  }
  
  return $output;
}
*/