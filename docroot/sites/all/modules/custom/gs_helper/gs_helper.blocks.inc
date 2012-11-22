<?php
/**
 * @file
 * Contains code specific to block functionality.
 */

/**
 * Implementation of hook_block_info();
 */
function gs_helper_block_info() {
  $blocks['logo'] = array(
    'info'  => t('Site Logo'), 
    'cache' => DRUPAL_CACHE_GLOBAL,
  );
  
  $blocks['today'] = array(
    'info'  => t("Today's Date"),
    'cache' => DRUPAL_NO_CACHE,
  );
  
  $blocks['section_header'] = array(
    'info'  => t('Section Header'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );

  $blocks['related_stories'] = array(
    'info'  => t('Related Stories'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['featured_articles'] = array(
    'info'  => t('Featured Articles'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['top_articles'] = array(
    'info'  => t('Top Articles'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['choices'] = array(
    'info'  => t('Choices'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['embedded_sidebar'] = array(
    'info'  => t('Embedded Sidebar'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['legacy_sidebar'] = array(
    'info'  => t('Legacy Sidebar'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['photos_lead_in'] = array(
    'info'  => t('Photos Lead-In'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );

  $blocks['videos_lead_in'] = array(
    'info'  => t('Videos Lead-In'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  $blocks['blog_topics_tray'] = array(
    'info'  => t('Blog Topics Tray'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['article_topics_tray'] = array(
    'info'  => t('Article Topics Tray'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['front_headlines'] = array(
    'info'  => t('Front Headlines'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['prod_tabs'] = array(
    'info'  => t('Product Tabs'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['prod_header'] = array(
    'info'  => t('Product Page Header'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['flickr_photos'] = array(
    'info'  => t('Flickr Photos'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['lgbt_external_links'] = array(
    'info'  => t('LGBT External Links'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['hot_topics'] = array(
    'info'  => t('Hot Topics'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  $blocks['tech_games_external_links'] = array(
    'info'  => t('Tech & Games External Links'),
    'cache' => DRUPAL_CACHE_PER_PAGE,
  );
  
  return $blocks;
}

/**
 * Implements hook_block_configure()
 */
function gs_helper_block_configure($delta = '') {
  $form = array();
  
  switch ($delta) {
    case 'article_topics_tray':
      break;
    
    default:
      break;
  }
  
  return $form;
}

/**
 * Implements hook_block_save()
 */
function gs_helper_block_save($delta = '', $edit = array()) {
  switch ($delta) {
    case 'article_topics_tray':
      break;
    
    default:
      break;
  }
}

/**
 * Implementation of hook_block_view();
 */
function gs_helper_block_view($delta = '') {
  if (path_is_admin(current_path())) {
   return;
  }
  
  $block = array();
  
  switch ($delta) {
    case 'logo':
      $block['content'] = array(
        '#theme'  => 'gs_helper_logo', 
        '#path'   => drupal_get_path('theme', 'gs') . '/img/misc/logo.png',
      );
      
      break;
      
    case 'today':
      $block['content'] = array(
        '#theme'  => 'gs_helper_today',
        '#day'    => date('l'),
        '#date'   => date('M j Y'),
      ); 
      
      break;
      
    case 'section_header':
      // if front page; make empty header 
      if (drupal_is_front_page()) {
        return;
      }
      // if front page draft; make empty header 
      if (drupal_match_path(current_path(), "home/*/draft")) {
        return;
      }
      
      $link = '';
      
      if ($node = menu_get_object()) {
        switch ($node->type) {
          case 'blog_post':
            $section = 'blogs';
            $link = 'blogra';
            break;
          case 'article':
            // Get the parent term of the node in the language they currently would be displayed.
            $terms = field_get_items('node', $node, 'field_section');
            $terms = taxonomy_get_parents_all(end(end($terms)));
            $parent_term = end($terms);
            break;
          case 'contest':
            $section = $link = 'contests';
            break;
          case 'content_classified_ad':
            $section = $link = 'classifieds';
            break;
        }
      }
      elseif ($term = menu_get_object('taxonomy_term', 2)) {
        $terms = taxonomy_get_parents_all($term->tid);
        $parent_term = end($terms);
      }
      elseif ($view = views_get_page_view()) {
        if($view->name == 'poll_listings') {
          $section = $link = 'polls';
        }
      }
      elseif (module_exists('gs_breadcrumb')) {
        $section = _gs_breadcrumb_get_menu_title_from_path(arg(0));
        $link = arg(0);
      }
      elseif ($panel = panels_get_current_page_display()) {
        // Use the panel title if no section is found.
        if ($panel_title = $panel->get_title()) {
          $section = $panel_title;
          $link = current_path();
        }
      }
      
      // If we're working with taxonomy terms, get the menu title it's mapped to.
      if (isset($parent_term) && module_exists('gs_breadcrumb')) {
        $path = _gs_breadcrumb_section_tid_map_search($parent_term->tid, TRUE);
        if($menu_title = _gs_breadcrumb_get_menu_title_from_path($path)) {
            $section = $menu_title;
            $link = $path;
          }
      }
      
      // Use a generic empty section header if no section is found.
      if (empty($section)) {
        $section = '';
        $header_class = 'default';
      } else {
        // Make section a machine readable string to be used as a css class.
        $header_class = preg_replace('@[^a-z0-9_]+@', '-', strtolower($section));
      }
      
      $block['content'] = array(
        '#theme'    => 'gs_helper_section_header',
        '#sections' => $section,
        '#link'     => $link,
        '#class'    => $header_class,
      );
      break;
      
    case 'blog_topics_tray':
      $terms = taxonomy_get_tree(5, 610);
      $active = array();
      
      if ($term_view = menu_get_object('taxonomy_term', 2)) {
        $active['tid'] = $term_view->tid;
      }
      
      if ($node = menu_get_object('node')) {
        // Get the last term of the list just in case more than one is returned.
        $active = end(field_get_items('node', $node, 'field_blog_term'));
      }
      
      if (isset($active['tid'])) {
        // Move the active term to the top of the list.
        foreach ($terms as $key => $term) {
          if ($term->tid == $active['tid']) {
            unset($terms[$key]);
            $terms = array_merge(array($term), $terms); 
            break;
          }
        }
      }
      
      $block['content'] = array(
        '#theme'  => 'gs_helper_blog_topics_tray',
        '#title'  => 'Blog Topics',
        '#topics' => $terms,
        '#active' => isset($active['tid']) ? $active['tid'] : FALSE,
      );
      break;
      
    case 'article_topics_tray':
      $mappedtids = _gs_helper_section_tid_map();
      if (isset($mappedtids[current_path()])) {
        $activeterm = taxonomy_term_load($mappedtids[current_path()]['tid'][0]);
      } else {
        $activeterm = menu_get_object('taxonomy_term',2);
      }
      if (is_object($activeterm)) {
        $parents = taxonomy_get_parents_all($activeterm->tid);
        $mainbranch = (count($parents) > 1 ? end($parents) : $activeterm);
      }
      if ($activeterm && $mainbranch) {
        // find children
        $terms = taxonomy_get_tree($mainbranch->vid, $mainbranch->tid);
        // Move the active term to the top of the list.
        foreach ($terms as $key => $term) {
          if ($term->tid == $activeterm->tid) {
            unset($terms[$key]);
            $terms = array_merge(array($term), $terms); 
            break;
          }
        }
        // prepare output
        $block['content'] = array(
          '#theme'  => 'gs_helper_blog_topics_tray',
          '#title'  => t('@main_branch_name Topics',array('@main_branch_name'=>$mainbranch->name)),
          '#topics' => $terms,
          '#active' => $activeterm->tid,
        );
      }
      break;
      
    case 'related_stories':
      if ($node = _gs_helper_node_from_url()) {
        if ($node->type == 'article') {
          $stories = array();
          
          if (isset($node->field_related_articles[$node->language][0])) {
            foreach ($node->field_related_articles[$node->language] as $item) {
              if ($item_node = node_load($item['nid'])) {
                $stories[$item_node->nid] = $item_node->title;
              }
            }
          }
          
          // we always want to display at least 3 related stories, so pull up
          // some nodes from the same section or topic
          /*if (($size_stories = sizeof($stories)) < 3 && isset($node->field_section[$node->language][0])) {
            
            $tid = $node->field_section[$node->language][0]['tid'];
            
            $term = taxonomy_term_load($tid);
            
            $parents = taxonomy_get_parents_all($tid);
            
            if (($size = sizeof($parents)) > 1) {
              $first = $parents[1];         // first parent
              $last = $parents[$size - 1];  // last parent
            }
            else {
              $first = $last = $parents[0];
            }
            
            // custom awesomesauce is awesome
            $tree = build_taxonomy_tree($last->vid, $last->tid);
            
            // get the term priority order
            $sort_data = array(
              'priority'  => array($tid),
              'immediate' => array(),
              'extended'  => array(),
              'with_kids' => array(),
              'uncles'    => array(),
            );
            
            $sort = _gs_helper_order_tids($tree, $tid, $first->tid, $sort_data);
            
            $terms = array_merge($sort['priority'], $sort['immediate']);
            $terms = array_merge($terms, $sort['extended']);

            if ($terms) {
              $order = array();
              foreach ($terms as $tid) {
                $order[] = 's.field_section_tid = ' . $tid . ' DESC';
              }

              $query = "  SELECT      DISTINCT(n.nid),
                                      n.title
                          FROM        {node} n
                          INNER JOIN  {field_data_field_section} s
                            ON        n.nid = s.entity_id
                            AND       s.entity_type = 'node'
                            AND       s.bundle = 'article'
                          WHERE       n.nid != :nid
                            AND       s.field_section_tid IN (:terms)
                          ORDER BY    n.created DESC, " . implode(', ', $order) . "
                          LIMIT       " . (3 - $size_stories) . "
                       ";

              $result = db_query($query, array(':terms' => $terms, ':nid' => $node->nid));

              foreach ($result as $row) {
                $stories[$row->nid] = $row->title;
              }
            }
          }*/
          
          if (!empty($stories)) {
            $block['subject'] = t('Related Stories');
            $block['content'] = array(
              '#theme'    => 'gs_helper_related_stories',
              '#stories'  => $stories,
          );  
          }
        }
      }
      
      break;
    case 'embedded_sidebar':
      if ($node = _gs_helper_node_from_url()) {
        if (isset($node->embedded_sidebar)) {
          $block['subject'] = !empty($node->embedded_sidebar['title']) ? $node->embedded_sidebar['title'] : '<none>';
          $block['content'] = $node->embedded_sidebar['content'];
        }
      }
      break;
      
    case 'legacy_sidebar':
      if ($node = _gs_helper_node_from_url()) {
        if (isset($node->field_legacy_sidebar['und'][0]['value'])) {
          $block['subject'] = isset($node->field_legacy_sidebar_title['und'][0]['value']) ? $node->field_legacy_sidebar_title['und'][0]['value'] : '<none>';
          $block['content'] = $node->field_legacy_sidebar['und'][0]['value'];
        }
      }
      break;
      
    case 'photos_lead_in':
      if ($vars['entity'] = _gs_helper_node_from_url()) {
        $vars['image_style_name'] = 'article_sidebar_thumbnail';
        if ($thumb = theme('gs_helper_node_thumbnail', $vars)) {
          $block['subject'] = t('Photos');
          $block['content']  = "<div class=\"photos_lead_in-block\">";
          // $block['content'] .= "<a alt=\"Photos\" title=\"Photos\" href=\"?qt-article-tabs=1#qt-article-tabs\">";
          $block['content'] .= "<a alt=\"Photos\" title=\"Photos\" href=\"#\">";
          $block['content'] .= $thumb;
          $block['content'] .= "<span class=\"icon\"></span>";
          $block['content'] .= "</a></div>";
        }
      }
      break;
      
    case 'videos_lead_in':
      if ($vars['entity'] = _gs_helper_node_from_url()) {
        $thumbpath = NULL;
        // look for videos in mediacollection field
        if (field_get_items('node', $vars['entity'], 'field_mediacollection')) {
          $ids = _gs_helper_extract_select_values($vars['entity']->field_mediacollection);
          $fieldcollection = array_sum($ids) ? entity_load('field_collection_item',$ids) : array();
          foreach ($fieldcollection as $key => $entity) {
            if (isset($entity->field_media['und']) && $entity->field_media['und'][0]['type'] == 'video') {
              $scheme = file_uri_scheme($entity->field_media['und'][0]['uri']);
              if (in_array($scheme, array('youtube', 'vimeo'))) {
                $wrapper = file_stream_wrapper_get_instance_by_uri($entity->field_media['und'][0]['uri']);
                $thumbpath = $wrapper->getLocalThumbnailPath();
                // $thumbpath = $wrapper->getOriginalThumbnailPath();
              } else {
                $thumbpath = $entity->field_media['und'][0]['uri'];
              }
              break;
            }
          }
        }
        if ($thumbpath) {
          $thumb = theme('image_style', array(
            'style_name' => 'article_sidebar_thumbnail',
            'path' => $thumbpath,
            'alt' => 'Videos',
            'title' => 'Videos',
          ));
        // embedded videos
        } elseif (isset($vars['entity']->embedded_videos_thumbnail)) {
          $thumbpath = $vars['entity']->embedded_videos_thumbnail;
          $thumb = theme('image',array(
            'path' => $thumbpath,
            'width' => '160',
            'height' => '120',
            'alt' => 'Videos',
            'title' => 'Videos',
          ));
        }
        if (isset($thumb)) {
          $block['subject'] = t('Videos');
          $block['content']  = "<div class=\"videos_lead_in-block\">";
          // $block['content'] .= "<a alt=\"Videos\" title=\"Videos\" href=\"?qt-article-tabs=2#qt-article-tabs\">";
          $block['content'] .= "<a alt=\"Videos\" title=\"Videos\" href=\"#\">";
          $block['content'] .= $thumb;
          $block['content'] .= "<span class=\"icon\"></span>";
          $block['content'] .= "</a></div>";
        }
      }
      break;
      
    // featured headlines  
    case 'front_headlines':
      $block['content'] = theme('gs_helper_front_headlines',array('items'=>variable_get('gs_helper_news_rotator_values')));
      break;
    
    // product page tabs  
    case 'prod_tabs':
      $block['content'] = theme('gs_helper_prod_tabs',array('items'=>variable_get('gs_helper_prod_tabs_values')));
      break;
    
    // LGBT external links
    case 'lgbt_external_links':
      $block['content'] = theme('gs_helper_lgbt_external_links',array('items'=>variable_get('gs_helper_lgbt_external_links')));
      break;

    // Hot Topics
    case 'hot_topics':
      $block['content'] = theme('gs_helper_lgbt_external_links',array('items'=>variable_get('gs_helper_hot_topics')));
      break;

    // Tech & Games external links
    case 'tech_games_external_links':
      $block['content'] = theme('gs_helper_lgbt_external_links',array('items'=>variable_get('gs_helper_techgames')));
      break;
    
    // product header (breadcrumb and shopping cart)
    case 'prod_header':
      $block['content'] = '<div class="product-breadcrumb-cart-wrapper">';
      $block['content'] .= '<div class="product-breadcrumb">';
      // breadcrumb
      $breadcrumb = array();
      if ($_GET['q'] == 'store') {
        $breadcrumb[] = l(t('Home'),'');
        $breadcrumb[] = t('Store');
      } else {
        $breadcrumb[] = l(t('Home'),'');
        $breadcrumb[] = l(t('Store'),'store');
        $breadcrumb[] = page_title_get_title();
      }
      $block['content'] .= theme('breadcrumb',array('breadcrumb'=>$breadcrumb));
      $block['content'] .= '</div>';
      // shopping cart
      global $user;
      if ($order = commerce_cart_order_load($user->uid)) {
        if ($wrapper = entity_metadata_wrapper('commerce_order', $order)) {
          $block['content'] .= '<div class="product-cart">';
          // quantity
          $qty = commerce_line_items_quantity($wrapper->commerce_line_items, commerce_product_line_item_types());
          $qty ? $block['content'] .= l("<span class=\"cart-quantity\">SHOPPING CART ($qty)</span>",'cart',array('html'=>TRUE)) : NULL;
          // amount
          $amount = commerce_line_items_total($wrapper->commerce_line_items, commerce_product_line_item_types());
          $currency = commerce_currency_load($amount['currency_code']);
          $value = commerce_currency_amount_to_decimal($amount['amount'], $amount['currency_code']);
          $formatted_value = number_format(commerce_currency_round(abs($value), $currency), $currency['decimals'], $currency['decimal_separator'], $currency['thousands_separator']);
          $value ? $block['content'] .= "<span class=\"cart-amount\">SUBTOTAL \$$formatted_value</span>" : NULL;
          $block['content'] .= '</div>';
        }
      }
      $block['content'] .= '</div>';
      break;
    
    // flickr photos block
    case 'flickr_photos':
      $block['content'] = '';
      require_once DRUPAL_ROOT . '/' . drupal_get_path('module','gs_helper') . '/phpFlickr/phpFlickr.php';
      try {
        $f = new phpFlickr('d610d6179d22ed48102844639753397d','da71a5bde93a85e7');
        $nsid = '46695849@N02';
        $recent = $f->people_getPublicPhotos($nsid,NULL,'description');
        if (isset($recent['photos']['photo'])) {
          $i = 0;
          $items = array();
          foreach ($recent['photos']['photo'] as $photo) {
            if ($i == 2) // amount of photos to list; TODO: make this value configurable through UI
              break;

            $path = "http://farm{$photo['farm']}.staticflickr.com/{$photo['server']}/{$photo['id']}_{$photo['secret']}_m.jpg";
            $html = theme('imagecache_external', array(
              'path' => $path, 
              'style_name' => 'sidebar_featured_thumbnail',
              'alt' => $photo['description'],
              'title' => $photo['description'],
            ));
            $html .= '<span class="icon"></span>';
            $data  = l($html, "http://www.flickr.com/photos/$nsid/{$photo['id']}",array('html'=>true));
            $data .= '<span class="description">'.$photo['title'].'</span>';
            $items[] = $data;
            $i++;
          }
          $block['content'] = theme('item_list',array('items'=>$items)); 
        }
      } catch(Exception $e) {
        watchdog_exception('gs_helper', $e);
        watchdog('gs_helper', $f->getErrorCode() .'<br />'. $f->getErrorMsg());        
      }
      break;
  }

  return $block;
}

/**
 * Implementation of hook_block_view_alter();
 */
function gs_helper_block_view_alter(&$data, $block) {
  
}

/**
 * Helper to generate the block structure for a particular View + display + arguments combination
 */
function _gs_helper_build_view_block($view_id, $display, $subject = '', $arguments = array()) {
  $block = array();

  if ($view = views_get_view($view_id)) {
    if ($view->access($display)) {
      $view->set_display($display);
      $view->set_arguments($arguments);

      $view_output = $view->preview();
      if (!empty($view->result) || $view->display_handler->get_option('empty') || !empty($view->style_plugin->definition['even empty'])) {
        $block['subject'] = check_plain($subject);
        $block['content']['#markup'] = $view_output;
      }
    }
    $view->destroy();
  }
  
  return $block;
}