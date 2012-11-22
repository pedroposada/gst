<?php
/**
 * @file
 * Contains code specific to theming functionality.
 */

/**
 * Content for logo block.
 */
function theme_gs_helper_logo($data) {
  $output = theme('image', array('path' => $data['path']));
  $output = l($output, '', array('html' => TRUE));
  
  return $output;
}

/**
 * Content for today's date block.
 */
function theme_gs_helper_today($data) {
  $output = '<div class="day">' . $data['day'] . '</div>';
  $output .= '<div class="date">' . $data['date'] . '</div>';
  
  return $output;
}

/**
 * Content for section header block.
 */
function theme_gs_helper_section_header($data) {
  // don't show block on the home page
  if (drupal_is_front_page()) {
    return '';
  }
  
  $output = '';
  $title = (strtolower($data['sections']) === 'blogs' ? 'blogra' : $data['sections']);
  $link = $data['link'];
  
  $output .= '<h1 id="section-header" class="' . $data['class'] . '">';
  $output .= empty($link) ? $title : l($title, $link);
  $output .= '</h1>';
  
  return $output; 
} 

/**
 * Content for related content block.
 */
function theme_gs_helper_related_stories($vars) {
  $output = '';
  
  if (!empty($vars['stories'])) {
    $items = array();
    
    foreach ($vars['stories'] as $nid => $title) {
      $items[] = l($title, 'node/' . $nid);
    } 
    
    $output .= theme('item_list', array('items' => $items));
  }
  
  return $output;
}

/**
 * Topics tray for Articles and Blogs
 */
function theme_gs_helper_blog_topics_tray($data) {
  $tree = menu_tree_all_data('main-menu');
  $paths = array();
  foreach ($tree as $key => $value) {
    if (isset($value['below']) && count($value['below'])) {
      foreach ($value['below'] as $below) {
        $paths[strtolower($below['link']['title'])] = $below['link']['link_path'];    
      }
    }
  }
  
  $options = $classes = array();
  $output = '<div class="title">' . $data['title'] . '</div>';
  $output .= '<ul id="topics-tray">';
  foreach ($data['topics'] as $topic) {
    // Replace taxonomy paths with menu paths if available, except for Blog Topics.
    $path = (isset($paths[strtolower($topic->name)]) && $topic->vid != 5) ? $paths[strtolower($topic->name)] : 'taxonomy/term/' . $topic->tid;
    $output .= '<li id="tag-' . $topic->tid . '">' . l( $topic->name, $path, ($topic->tid == $data['active'] ? array('attributes' => array('class' => array('active'))) : array()) ) . '</li>';
  }
  $output .= '</ul>';
  $output .= l(t('more'), $_GET['q'], array('attributes' => array('class' => array('more','collapsed')), 'fragment' => 'topics-tray'));
  
  return '<div id="topics-tray-wrapper">'.$output.'</div>';
}

/**
 * Field formatter - GS Photo Item
 */
function theme_gs_helper_field_media_photo($vars) {
  $output = '';
  
  $file = $vars['file'];
  
  $image_vars = array(
    'path' => $file['uri'],
    'alt' => '',
    'title' => $file['filename'],
    'style_name' => 'slideshow',
  );
  
  if (!empty($file['field_cutline']['und'][0]['safe_value'])) {
    $image_vars['title'] = $file['field_cutline']['und'][0]['safe_value'];
  }

  if (!empty($file['field_credit']['und'][0]['safe_value'])) {
    $image_vars['alt'] = $file['field_credit']['und'][0]['safe_value'];
  }
  
  $output = theme('image_style', $image_vars);

  return $output;
}

/**
 * Field formatter - GS Video Item
 */
function theme_gs_helper_field_video($vars) {
  $output = '';

  if (!empty($vars['image_file'])) {
    $items['movie'] = (array) $vars['video_file'];
  
    $items['poster'] = (array) $vars['image_file'];
    
    $video_vars = array(
      'items'       => $items,
      'attributes'  => array(
        'width' => 600,
        // 'height' => 300,
      ),
      'player_id'   => 'file-' . $vars['video_file']['fid'],
    );
    
    $output = '<div class="video-content">' . theme('videojs', $video_vars) . '</div>';
  }
  
  return $output;
}

/**
 * Field formatter - Image Preset
 */
function theme_gs_helper_field_image_preset($vars) {
  $output = '';
  
  $file = $vars['file'];
  $url = file_create_url($file['uri']);
  
  if (!empty($file['uri'])) {
    $image_vars = array(
      'path' => $file['uri'],
      'alt' => '',
      'title' => $file['filename'],
      'style_name' => $vars['image_style'],
    );
    
    if (isset($file['not_stream']) && $file['not_stream']) {
      $output = '<img src="' . $file['uri'] . '" />';
    }
    else {
      $output .= theme('image_style', $image_vars);
    }
    
    if ($vars['markup']) {
      $output .= $vars['markup'];
    }
    
    if ($vars['cutline']) {
      // cutline field & link
      if (!empty($file['field_cutline']['und'][0])) {
        $cutline = $file['field_cutline']['und'][0]['value'];
        
        if (!empty($file['field_cutline_link']['und'][0]['value'])) {
          $link = $file['field_cutline_link']['und'][0]['value'];
          ensure_http($link);
          
          $cutline = l($cutline, $link);
        }
        
        $output .= '<div class="cutline">' . $cutline . '</div>';
      }

      // credit field & link
      if (!empty($file['field_credit']['und'][0])) {
        $credit = $file['field_credit']['und'][0]['value'];
        
        if (!empty($file['field_credit_link']['und'][0]['value'])) {
          $link = $file['field_credit_link']['und'][0]['value'];
          ensure_http($link);
          
          $credit = l($credit, $link);
        }
        
        $output .= '<div class="credit">' . $credit . '</div>';
      }
    }
    
    $filename = '';
    if ($vars['filename']) {
      $filename = '<div class="filename">' . $vars['filename'] . '</div>';
    }
    
    if (isset($vars['link']) && !empty($vars['link'])) {
      $output = l($output, $vars['link'], array('html' => TRUE));
      
      if ($filename) {
        $filename = l($filename, $vars['link'], array('html' => TRUE));
      }
    }
    
    $output .= $filename;
  }
  
  return $output;
}

/**
 * Media Browser
 * $vars
 *  - entity
 */
function theme_gs_helper_media_browser($vars) {
  $output = $items = NULL;
  
  if (isset($vars['entity'])) {
    if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
      $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
      $fieldcollection = entity_load('field_collection_item',$ids);
      foreach ($fieldcollection as $key => $entity) {
      //dsm($entity);
        if (isset($entity->field_media['und'])) {
          $data = array(
            'cutline' => (isset($entity->field_cutline['und']) ? $entity->field_cutline['und'][0]['value']: NULL),
            'credit' => (isset($entity->field_credit['und']) ? $entity->field_credit['und'][0]['value']: NULL),
            'fid' => $entity->field_media['und'][0]['fid'],
          );
          $variables = array(
            'style_name' => 'thumbnail_gsmediabrowser',
            'alt' => $data['cutline'],
            'title' => $data['credit'],
          );
          $scheme = file_uri_scheme($entity->field_media['und'][0]['uri']);
          if (in_array($scheme, array('youtube', 'vimeo'))) {
            $wrapper = file_stream_wrapper_get_instance_by_uri($entity->field_media['und'][0]['uri']);
            $variables['path'] = $wrapper->getLocalThumbnailPath();
            $items[] = array('#markup' => theme('image_style', $variables), 'data' => $data, 'class'=>$entity->field_media['und'][0]['type']);
          } else {
            $variables['path'] = $entity->field_media['und'][0]['uri'];
            $items[] = array('#markup'=>theme('image_style',$variables), 'data' => $data, 'class' => 'image');  
          }
        }
      }
    }
    // grab images from attached gallery if any
    if (isset($vars['entity']->field_gallery['und'])) {
      $gallery = node_load($vars['entity']->field_gallery['und'][0]['nid']);
      if (isset($gallery->field_mediacollection['und'])) {
        $ids = _gs_helper_extract_select_values($gallery->field_mediacollection);
        $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
        foreach ($fieldcollection as $key => $entity) {
          if (isset($entity->field_media['und'])) {
            $data = array(
              'cutline' => (isset($entity->field_cutline['und']) ? $entity->field_cutline['und'][0]['value']: NULL),
              'credit' => (isset($entity->field_credit['und']) ? $entity->field_credit['und'][0]['value']: NULL),
              'fid' => $entity->field_media['und'][0]['fid'],
            );
            $variables = array(
              'style_name' => 'thumbnail_gsmediabrowser',
              'alt' => $data['cutline'],
              'title' => $data['credit'],
              'path' => $entity->field_media['und'][0]['uri'],
            );
            $items[] = array('#markup'=>theme('image_style',$variables), 'data' => $data);
          }
        }
      }
    }
  }
  if (count($items)) {
    $output .= '<div class="media_browser_list">';
    foreach ((array)$items as $key => $value) {
      $output .= '<div class="gsmedia-item-wrapper" id="'.$value['data']['fid'].'">';
        $output .= "<div class=\"gsmedia-item {$value['class']}\">";
        $output .= $value['#markup'];
        $output .= '</div>';
        $output .= '<div class="cutline">';
        $output .= $value['data']['cutline'];
        $output .= '</div>';
        $output .= '<div class="credit">';
        $output .= $value['data']['credit'];
        $output .= '</div>';
      $output .= '</div>';
    }
    $output .= '</div>';
  } else {
    $output  = '<p>'.t("Looks like there are no files associated with this content yet.").'</p>';
    $output .= '<p>'.t("Plase add media files to this content first.").'</p>';
  }
  
  return $output;
}
/**
 * render single file for inline media insert in wysiwyg
 */
function theme_gs_helper_inline_media($vars) {
  $output = '';
  $file = $vars['file'];
  
  if ($file->type == 'image') {
    $markup = theme('image_style',array(
      'style_name'=>'inline_wysiwyg',
      'path' => $file->uri,
      'alt' => $file->alt,
    ));
  } else {
    switch ($file->filemime) {
      case 'video/youtube':
        // $video = theme('media_youtube_video',array('uri'=>$file->uri,'width'=>420));
        $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
        $parts = $wrapper->get_parameters();
        $video_id = check_plain($parts['v']);
        $variables = array(
          'element' => array(
            '#tag' => 'iframe',
            '#attributes' => array(
              'src' => "http://www.youtube.com/v/$video_id?version=3&enablejsapi=1",
              'width' => 420,
              'height' => 315,
            ),
          ),
        );
        $video = theme('html_tag', $variables);
        break;
      case 'video/vimeo':
        $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
        $parts = $wrapper->get_parameters();
        $video_id = check_plain($parts['v']);
        $video = '<iframe src="http://player.vimeo.com/video/' . $video_id . '?byline=0&amp;portrait=0&amp;color=ee3a43" width="420" height="236" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe>';
        break;
      default:
        $video_vars = array(
          'items'       => array(
            'movie' => $featured,
            'poster' => (isset($file->field_thumbnail['und']) ? $file->field_thumbnail['und'][0]:array()),
          ),
          'attributes'  => array(
            'width' => 420,
          ),
          'player_id'   => 'file-' . $file->fid,
        );
        $video = theme('videojs', $video_vars);
        break;
    }
    $markup = $video;
  }
  
  $output .= '<div class="gsmedia-item-wrapper" id="'.$file->fid.'">';
    $output .= '<div class="gsmedia-item">';
    $output .= $markup;
    $output .= '</div>';
    $output .= '<div class="cutline">';
    $output .= '</div>';
    $output .= '<div class="credit">';
    $output .= '</div>';
  $output .= '</div>';
  
  return $output;
}

/*************************************************************
 * front page headlines admin form
 */
function theme_gs_helper_news_rotator_form($variables) {
  $output = '';
  $form = $variables['form'];
  $output .= drupal_render($form['preview_pane']);
  $rows = $weights = array();
  foreach (element_children($form['headlines']) as $id) {
    $form['headlines'][$id]['weight']['#attributes']['class'] = array('headlines-item-weight');
    $rows[] = array(
      'data' => array(
        drupal_render($form['headlines'][$id]['headline']),
        drupal_render($form['headlines'][$id]['url']),
        drupal_render($form['headlines'][$id]['remove']),
        drupal_render($form['headlines'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
    $weights[] = $form['headlines'][$id]['weight']['#value'];
  }
  $header = array(t('Headline'), t('Link'), t('Operations'), t('Weight'));
  $table_id = 'headlines-items-table';
  array_multisort($weights, $rows);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'headlines-item-weight');
  return $output;
}
/**
 * theme front headlines items
 */
function theme_gs_helper_front_headlines($variables) {
  $output = '';
  if (isset($variables['items']) && count($variables['items'])) {
    $output .= '<div class="front-page-headlines-wrapper">';
    $items = array();
    foreach ($variables['items'] as $key => $value) {
      $items[] = l($value['headline'],$value['url']);
    }
    $output .= '<div class="title">FEATURED</div>';
    // $variables['class'] = array('front-page-headlines');
    $output .= theme('item_list',array('items'=>$items,'attributes'=>array('class'=>array('front-page-headlines'))));
    $output .= '</div>';
  }
  return $output;
}
/*************************************************************
 * product tabs admin form
 */
function theme_gs_helper_prod_tabs_form($variables) {
  drupal_add_js(drupal_get_path('module', 'gs_helper') . '/js/gs_helper.forms.js');
  $output = '';
  $form = $variables['form'];
  $output .= drupal_render($form['message']);
  $output .= drupal_render($form['preview_pane']);
  $rows = $weights = array();
  foreach (element_children($form['tabs']) as $id) {
    $form['tabs'][$id]['weight']['#attributes']['class'] = array('prod-tabs-item-weight');
    $rows[] = array(
      'data' => array(
        drupal_render($form['tabs'][$id]['tabtitle']),
        array('data' => drupal_render($form['tabs'][$id]['tabdescription']), 'width'=>'70%'),
        drupal_render($form['tabs'][$id]['remove']),
        drupal_render($form['tabs'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
    $weights[] = $form['tabs'][$id]['weight']['#value'];
  }
  $header = array(t('Title'), t('Description'), t('Operations'), t('Weight'));
  $table_id = 'prod-tabs-items-table';
  array_multisort($weights, $rows);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'prod-tabs-item-weight');
  return $output;
}
/**
 * theme product tabs
 */
function theme_gs_helper_prod_tabs($variables) {
  $output = '';
  if (isset($variables['items']) && count($variables['items'])) {
    $output .= '<div class="prod-tabs-wrapper">';
    // Constructs quicktabs instance.
    foreach ($variables['items'] as $key => $value) {
      $tabs[] = array('title' => $value['tabtitle'], 'contents' => array('#markup' => $value['tabdescription']['value']), 'weight' => $value['weight']);
    }
    $style = isset($variables['style']) ? $variables['style'] : 'nostyle';
    $settings = array('style' => $style, 'sorted' => TRUE, 'ajax' => FALSE);
    $output .= render(quicktabs_build_quicktabs('prod-tabs', $settings, $tabs));
    $output .= '</div>';
  }
  return $output;
}
/********************************************************************
 * lgbt_external_links admin form
 */
function theme_gs_helper_lgbt_external_links_form($variables) {
  drupal_add_js(drupal_get_path('module', 'gs_helper') . '/js/gs_helper.forms.js');
  $output = '';
  $form = $variables['form'];
  $output .= drupal_render($form['help']);
  $output .= drupal_render($form['preview_pane']);
  $rows = $weights = array();
  foreach (element_children($form['tabs']) as $id) {
    $form['tabs'][$id]['weight']['#attributes']['class'] = array('prod-tabs-item-weight');
    $rows[] = array(
      'data' => array(
        '', // dargging handle
        drupal_render($form['tabs'][$id]['tabtitle']) .
        drupal_render($form['tabs'][$id]['tablink']) .
        drupal_render($form['tabs'][$id]['tabsource']),
        drupal_render($form['tabs'][$id]['remove']),
        drupal_render($form['tabs'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
    $weights[] = $form['tabs'][$id]['weight']['#value'];
  }
  // table headers
  $header = array(
    '', // dragging handle
    t('Info'), 
    t('Operations'),
    t('Weight'),
  );
  $table_id = 'prod-tabs-items-table';
  array_multisort($weights, $rows);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'prod-tabs-item-weight');
  return $output;
}
/**
 * lgbt_external_links
 */
function theme_gs_helper_lgbt_external_links($variables) {
  $output = '';
  if (isset($variables['items']) && count($variables['items'])) {
    $output .= '<div class="lgbt-external-links-wrapper">';
    // prepare list items
    $items = array();
    foreach ($variables['items'] as $item) {
      $link  = '<div class="external-link">';
      $link .= l($item['tabtitle'],$item['tablink'],array('attributes'=>array('target'=>'_blank')));
      $link .= '</div>';
      $link .= '<div class="source-page">';
      $link .= "{$item['tabsource']} &raquo;";
      $link .= '</div>';
      $items[] = $link;
    }
    $output .= theme('item_list', array('items'=>$items));
    $output .= '</div>';
  }
  return $output;
}