<?php
/**
 * @file
 * Adds new 'Section' condition to the Context module.
 */

/**
 * Expose paths as a context condition.
 */
class gs_helper_context_condition_section extends context_condition {
  /**
   * Omit condition values. We will provide a custom input form for our conditions.
   */
  function condition_values() {
    $sections = array(
      'blogs'       => t('Blogs'),
      'news'        => t('News'),
      'music'       => t('Music'),
      'arts'        => t('Arts'),
      'movies'      => t('Movies'),
      'food'        => t('Food'),
      'life'        => t('Life'),
      'listings'    => t('Listings'),
      'contests'    => t('Contests'),
      'store'       => t('Store'),
      'classifieds' => t('Classifieds'),

    );
    
    return $sections;
  }
  
  /**
   * Execute.
   */
  function execute() {
    if ($this->condition_used()) {
      foreach ($this->get_contexts() as $context) {
        foreach ($this->fetch_from_context($context, 'values') as $section) {
          $met = FALSE;
          
          $path = $section;
          $node_types = array(
            $section
          );
          
          if (arg(0) == $path) {
            $met = TRUE;
          }
          elseif (preg_match('#^node/([0-9]+)#', $_GET['q'], $matches)) {
            if ($node = node_load($matches[1])) {
              switch ($section) {
                case 'blogs':
                  if ($node->type == 'blog_post') {
                    $met = TRUE;
                  }
                
                  break;
                
                case 'news':
                case 'music':
                case 'arts':
                case 'movies':
                case 'food':
                case 'life':
                  if ($node->type == 'article') {
                    
                  }
                  
                  break;
                  
                case 'listings':
                  
                  break;
                  
                case 'contests':
                  
                  break;
                  
                case 'store':
                  
                  break;
                  
                case 'classifieds':
                  
                  break;
              }
              
              if (in_array($node->type, $node_types)) {
                $met = TRUE;
              }
            }
          }

          if ($met) {
            $this->condition_met($context);
          }
        }
      }
    }
  }
}
