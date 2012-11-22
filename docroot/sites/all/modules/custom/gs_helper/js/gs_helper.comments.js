(function($) {
  $(document).ready(function() {
    var allowedcharacters = 2400;
    
    init_counter($('div[id^="edit-comment-body"] textarea'));
    
    // Hide "Add new comment" if there are no comments
    if ($('.comment').length) {
      $('h2.comment-form').show();
    } else {
      $('h2.comment-form').hide();
    }
    
    $('#comment-wrapper').ajaxComplete(function(event, request, settings) {
      if (request.statusText === 'success' && settings.url === '/system/ajax') {
        var comments = $('h2.comments-header span.count').html().match(/\d+/);
        comments = parseInt(comments[0]);
        comments++;
        
        // Update comment counts to reflect newly submitted comment.
        $('h2.comments-header span.count').html('(' + comments + ')');
        $('#widgets-element-gs-comment-button-count a span span').html(comments);
        // Show comment form title if it's hidden.
        $('h2.comment-form').show();
        
        // When a comment is added through ajax, the counter dissapears.
        init_counter($('div[id^="edit-comment-body"] textarea'));
      }
    });
    
    function init_counter(textarea) {
      if (textarea.length) {
        // Check if the counter element exists.
        if (textarea.parent('.form-textarea-wrapper #alert-container-comments-body').length == 0) {
          // We append the counter to the end of the textarea wrapper so not to conflict with the grippie.
          textarea.parent('.form-textarea-wrapper').append('<div id="alert-container-comments-body">You have <span>' + allowedcharacters + '</span> characters remaining.</div>');
        }
        
        textarea.bind('keyup change', function(e) {
          update_chars_left(allowedcharacters, textarea[0], $('#alert-container-comments-body span'));
        });
      }
    }
    
    function update_chars_left(max_len, target_input, display_element) {
      var text_len = target_input.value.length;
      if (text_len >= max_len) {
        target_input.value = target_input.value.substring(0, max_len); // truncate
        display_element.html('0');
      } else {
        display_element.html(max_len - text_len);
      }
    }
    
  });
})(jQuery);