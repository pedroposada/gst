<?php
/**
 * @file
 * Contains code specific to the Quicktabs module.
 */

/**
 * Implemetation of hook_quicktabs_alter();
 */
function gs_helper_quicktabs_alter(&$quicktabs) {
  if (path_is_admin(current_path())) {
    return;
  }
  
  switch ($quicktabs->machine_name) {
    case 'most_popular':
      $section_map = _gs_helper_section_tid_map();
      $type = 'all';
      
      // Mapping of blog topics that should also appear for Sections
      $blog_map = _gs_helper_blog_tid_map();
      
      // Check if it's a mapped section.
      if (isset($section_map[current_path()]) && $section_map[current_path()]['tid'][0] != -1) {
        $parents = taxonomy_get_parents_all($section_map[current_path()]['tid'][0]);
        $terms[] = end($parents)->tid;
        
        /**************************************************
        // IMPORTANT NOTE
        // Following code was removed based on editor feedback
        // They wanted the most popular block to function like the following:
        //
        // - When you are viewing a Section or Subsection page, show the most 
        //   popular articles AND blogs from ONLY the Section.
        // - EXCEPTION: Books, LGBT, Style, Tech & Games will show articles
        //              AND blogs from their respective Subsection.
        
        if (in_array(610, $terms)) {
          $type = 'blog_post';
        }
        else {
          $type = 'article';
        }
        **************************************************/
      }
      // Check if it's a term_view page.
      elseif (arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2))) {
        $parents = taxonomy_get_parents_all(arg(2));
        $term = end($parents);
        
        // Check if it's a article or blog term view
        if (in_array($term->vid, array(2, 5))) {
          $terms[] = $term->tid;
        }
        
        /**************************************************
        // IMPORTANT NOTE
        // Following code was removed based on editor feedback
        // They wanted the most popular block to function like the following:
        //
        // - When you are viewing any Taxonomy page, show the most popular 
        //   articles AND blogs from ONLY the Section.
        
        $terms[] = arg(2);
        
        $term = taxonomy_term_load(arg(2));
        
        switch ($term->vocabulary_machine_name) {
          case 'blog':
            $type = 'blog_post';
            break;
          case 'section':
            $type = 'article';
            break;
        }
        **************************************************/
      }
      // Check if it's a node.
      elseif (arg(0) == 'node' && is_numeric(arg(1)) && count(arg()) == 2) {        
        /**************************************************
        // IMPORTANT NOTE
        // Following code was removed based on editor feedback
        // They wanted the most popular block to function like the following:
        //
        // - When you are viewing an article or blog, show the most popular 
        //   articles AND blogs from ALL Sections.
        //
        // This behavior is handled by default further down the code so this 
        // refinement is commented out in case it needs to be reimplemented.
        
        $node = menu_get_object('node');
        $type = $node->type;
        
        switch ($node->type) {
          case 'article':
            // Get the main section terms from article
            $node_terms = field_get_items('node', $node, 'field_section');
            break;
          case 'blog_post':
            // Get the main section terms from blog
            $node_terms = field_get_items('node', $node, 'field_blog_term');
            break;
          default:
            $type = 'all';
            break;
        }
        
        // We want to format the results into something we can consistently process later.
        if (isset($node_terms)) {
          foreach ($node_terms as $term) {
            $terms[] = $term['tid'];
          }
        }
        **************************************************/
      }
      
      // Convert the array into contextual filter values seperated by +.
      if (isset($terms)) {
        // Loop through terms and check if a blog term should be included.
        foreach ($terms as $term) {
          if (isset($blog_map[$term])) {
            $terms[] = $blog_map[$term];
          }
        }
        
        $arguments['terms'] = implode('+', $terms);
      }
      else {
        // If no terms are assigned, it defaults to load all terms from Articles and Blogs.
        $all_terms = array_merge(taxonomy_get_tree(2, 0, 1), taxonomy_get_tree(5, 0, 1));
        
        foreach ($all_terms as $term) {
          $terms[] = $term->tid;
        }
        
        $arguments['terms'] = implode('+', $terms);
      }
      
      if (isset($type)) {
        $arguments['type'] = $type;
      }
      if (isset($depth)) {
        $arguments['depth'] = $depth;
      }
      
      // Contextual filter values should only contain numbers seperated by + for the most_popular view.
      if (preg_match('/^[0-9+]+$/', $arguments['terms'])) {
        foreach ($quicktabs->tabs as &$tab) {
          if ($tab['type'] == 'view' && $tab['vid'] == 'most_popular') {
            // Merge arguments for most_popular view.
            // Term ID (required), Content Type (optional, default: all), and Depth Modifier (option, default: 10)
            $tab['args'] = implode('/', $arguments);
          }
        }
      }
      break;
    
    case 'latest_stories':
      $quicktabs->tabs[0]['args'] = '';
      $quicktabs->tabs[1]['args'] = GS_TID_NEWS;
      $quicktabs->tabs[2]['args'] = GS_TID_MOVIES;
      $quicktabs->tabs[3]['args'] = GS_TID_MUSIC;
      $quicktabs->tabs[4]['args'] = GS_TID_FOOD;
      $quicktabs->tabs[5]['args'] = GS_TID_ARTS;
      $quicktabs->tabs[6]['args'] = GS_TID_BLOGS;
      break;
    
    default:
      
      break;
  }
}