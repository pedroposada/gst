<script>
  (function($){
    $(document).ready(function() {
      var ar = 160/120;
      $("#jcrop_target").Jcrop({
        onChange: showPreview,
        onSelect: showPreview,
        aspectRatio: ar,
        setSelect: [20,20,180,140],
      });
    });
    
    function showPreview(coords) {
      var rx = 160 / coords.w;
      var ry = 120 / coords.h;
    
      $("#jcrop_preview").css({
        width: Math.round(rx * $("#jcrop_target").width()) + "px",
        height: Math.round(ry * $("#jcrop_target").height()) + "px",
        marginLeft: '-' + Math.round(rx * coords.x) + "px",
        marginTop: '-' + Math.round(ry * coords.y) + "px",
      });
    }
  })(jQuery);
</script>

<?php
  $variables = array();
  if (isset($_GET['fid'])) {
    $file = file_load($_GET['fid']);
    $variables = array(
      'style_name' => 'cropper',
      'path' => $file->uri,
    );
  }
?>

<div class="gs-image-cropper-wrapper">
  
  <div id="jcrop-thumbnail-result">
    <div style="width:160px;height:120px;overflow:hidden;">
        <?php 
          $variables['attributes']['id'] = 'jcrop_preview';
          print theme('image_style',$variables); 
        ?>  
    </div>
  </div>
  <div class="description" style="clear: both"><p>Use cursor to crop image:</p></div>
  <div>
    <?php
      $variables['attributes']['id'] = 'jcrop_target';
      print theme('image_style',$variables);  
    ?>
  </div>
  
</div>