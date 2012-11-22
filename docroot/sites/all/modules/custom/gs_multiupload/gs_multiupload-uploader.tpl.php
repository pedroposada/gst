    <!-- Our CSS stylesheet file -->
    <link rel="stylesheet" href="/<?php print drupal_get_path('module','gs_multiupload'); ?>/css/styles.css" />
    <!--[if lt IE 9]>
      <script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
    <![endif]-->
    
    <header>
      <h2><?php print t("Upload multiple files to %g",array('%g'=>$node->title)) ?></h2>
      <p><?php print t("Drop %n images at a time",array('%n'=>variable_get('gs_multiupload_maxfiles',10))) ?></p>
    </header>
    
    <?php print $close_button ?>
    <div id="dropbox">
      <span class="message">Drop images here to upload. <br /><i>(they will only be visible to you)</i></span>
    </div>
    
    <!-- Including the HTML5 Uploader plugin -->
    <script src="/<?php print drupal_get_path('module','gs_multiupload'); ?>/js/jquery.filedrop.js"></script>
    
    <!-- The main script file -->
    <script src="/<?php print drupal_get_path('module','gs_multiupload'); ?>/js/script.js"></script>    