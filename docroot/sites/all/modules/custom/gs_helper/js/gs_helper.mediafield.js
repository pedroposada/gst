(function($) {
  Drupal.behaviors.gs_helper_mediafield = {
    attach: function (context, settings) {
      
    // Media tab in node edit form; For each widget (in case of multi-entry)
    $('.media-widget', context).once('gsMediaCollection', function () {
      // stuff from media module; media.js
      var options = settings.media.elements[this.id];
      globalOptions = {};
      if (options.global != undefined) {
        var globalOptions = options.global;
      }
      
      // initialize variables
      var fidField = $('.fid', this);
      var previewField = $('.preview', this);
      var previewMeta = $('.meta', this);
      // var editButton = $('.edit', this); // Actually a link, but looks like a button.
      var editButton = $('<a class="button edit" href="#">Add Thumbnail</a>'); // Actually a link, but looks like a button.
    $(this).append(editButton);
      
      // prepare for non-multiple value form
      if($(this).closest("table").length > 0) {
        var parentTable = $(this).closest("table");
        var parentTr = $(this).closest("tr");
      } else {
        var parentTable = $(this).closest("form");
        var parentTr = $(this).closest("form");
      }
      
      var thumbData = $('input[name*="field_thumbnaildata"]', parentTr);
      var cropData = $('input[name*="field_crop_data"]', parentTr);
      
      // hide URL Redirects fieldset
      $('.redirect-list', parentTr).hide();
      
      // hide thumbnail data field wrapper and crop data field wrapper
      $('div[class*="field-thumbnaildata"]', parentTr).hide();
      $('div[class*="field-crop-data"]', parentTr).hide();
      
      // hide/show thumbnail checkbox wrapper
      var thumbCheckbox = $('div[class*="field-thumbnailselected-und"]', parentTr);
      
      if (thumbData.val()) {
        editButton.show();  
        thumbCheckbox.show();
        thumbButtonText = 'Edit Thumbnail';
      }
      else {
        editButton.hide();  
        thumbCheckbox.hide();
        thumbButtonText = 'Add Thumbnail';
      }
      
      editButton.html(thumbButtonText);
        
      // set thumbnail container; show thumbnail in list
      var thumbImage = $('<div class="thumbnail-preview" />');
      thumbCheckbox.prepend(thumbImage);
      if (thumbData.val()) {
        thumbImage.html(thumbData.val());
      }
        
      // remove "select media" button once media has been selected
      $(this).bind("DOMSubtreeModified", function(){
        //console.log(this.id + ': DOMSubtreeModified');
        if (fidField.val() != "0") {
          editButton.show();  
          $('a.launcher', this).hide();
          $('div.launcher', this).unbind('click');
          $(this).unbind("DOMSubtreeModified");
          //console.log(this.id + ': unbind DOMSubtreeModified');
          
          // hide "add/edit thumbnail" button if its a video
          if ($('.media-item .media-thumbnail img[src*="vimeo"]', parentTr).length) {
            editButton.remove();
          }
          if ($('.media-item .media-thumbnail img[src*="youtube"]', parentTr).length) {
            editButton.remove();
            // $('.edit', this).hide();
            //console.log(this.id + ": remove edit button");
          }
        }
      });

      // hide "remove" button
      $('.remove', this).remove();
      
      // create object once; to attach dialog to it
      var myIframe = $('<div class="gs-image-cropper-contents" />');
      
      // unbind default click event for editbuttom
      editButton.unbind('click');
      
      // popup cropper window on click
      editButton.bind('click', function(){
        var fid = fidField.val();
        
        // callback to see if this is video or image
        $.getJSON('/gs/ajax/file-load/' + fid, function(file){
        // file = $.parseJSON(data);
          
          if (fid && file.type == 'image') {
            // alert(fid);
            
            // configure modal dialog for images
            myIframe.dialog({
              title: thumbButtonText,
              autoOpen: false,
              height: 850,
              width: 620,
              modal: true,
              buttons: {
                'Ok': function(){
                  $("#jcrop-thumbnail-result img").removeAttr('id');
                  var thumbHtml = $("#jcrop-thumbnail-result").html();
                  thumbData.val(thumbHtml);
                  thumbImage.html(thumbHtml).show();
                  thumbImage.bind('click', function(){ editButton.click(); });
                  thumbCheckbox.show();
                  $('input[name*="field_thumbnailselected"]', parentTr).once().attr('checked','checked').change();
                  thumbButtonText = 'Edit Thumbnail';
                  editButton.html(thumbButtonText);
                  $( this ).dialog( "close" );
                },
                'Cancel': function() {
                  $( this ).dialog( "close" );
                }
              },
              open: function(event, ui){
                thumbImage.unbind('click');
                $.get('/gs/ajax/image-cropper?fid=' + fid, function(data){
                  myIframe.html(data);
                });
                // remove media dialog and overlay
                $('.ui-widget-overlay:first').remove();
                $('.media-wrapper').remove();
              },
              close: function(event, ui){
                $( this ).dialog( "destroy" );
                $( this ).remove();
              },
            });
            
            // open modal dialog
            myIframe.dialog("open");
          }
          else if (fid) {
            // popup for video
            Drupal.media.popups.mediaFieldEditor(fid, function (r) {
              // alert(r); 
            });
            thumbImage.hide();
            thumbCheckbox.hide();
          }
          else {
            alert('Invalid File.');
          }
        });
        
        return false;
        });
        
        // bind actions to thumbnail selected checkbox 
        $('input[name*="field_thumbnailselected"]', parentTable).bind('change',function(){
          if ($(this).attr('checked')) {
            $('input[name*="field_thumbnailselected"]').not(this).removeAttr('checked');
          }
        });
      });
      
      /*********************************************************/
      // Downloads tab in node edit form; For each widget (in case of multi-entry)
      // $('#edit-field-files .media-widget', context).once('gsDownloadFiles', function () {
        // // initialize variables
        // var widget = $(this); 
        // var fidField = $('.fid', this);
        // var removeButton = $('.remove', this);
        // var selectMediaButton = $('.buttons a.launcher', this);
        // // hide "select media" button once media has been selected
        // // $(this).bind("DOMSubtreeModified", function(){
        // function toggleButtons(){
          // // console.log('DOMSubtreeModified triggered');
          // if (fidField.val() != "0") {
            // selectMediaButton.hide();
            // $('div.launcher', this).unbind('click');
            // $(this).unbind("DOMSubtreeModified");
          // }
        // };
        // widget.bind("DOMSubtreeModified",toggleButtons);
        // // hide "select media" by default
        // if (fidField.val() != "0") {
          // selectMediaButton.hide();
        // }
        // // show "select media" button on click
        // removeButton.bind('click', function(){
          // selectMediaButton.show();
          // widget.bind("DOMSubtreeModified",toggleButtons);
          // //console.log('removeButton triggered');
        // });
        // // hide "edit" button by default
        // $('.edit', this).remove();
      // });
      
      /****************************************************************/
      // WYSIWYG media button; For each widget (in case of multi-entry)
      // $('#wysiwygToolbar a.mce_media', context).once('gsWysiwygMediaButton', function () {
        // // this.unbind('click');
        // console.log("wysiwyg media button.");
        // // this.bind('click', function(){
        // // console.log("wysiwyg media button click.");
        // // })
      // });
    }
  };
})(jQuery);