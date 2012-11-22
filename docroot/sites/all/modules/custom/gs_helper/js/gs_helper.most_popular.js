(function ($) {
  // set up the 'more' link within most popular content block
  var most_popular = $('#block-quicktabs-most_popular, .pane-quicktabs-most-popular');
  
  if (most_popular.length) {
    var collapsable_container = $('.collapsable', most_popular);
    var expanded = false;
    
    if (collapsable_container.length) {
      $('h2 a.more', most_popular).click(function(e) {
        e.preventDefault();
        
        var trigger = $(this);
        
        if (expanded) {
          $('.collapsable:visible', most_popular).slideUp();
          // slideUp() does not hide the element if it's in a parent that's already hidden.
          // This is a workaround to make sure it's hidden.
          $('.collapsable:hidden', most_popular).hide();
          expanded = false;
          trigger.removeClass('expanded');
        }
        else {
          $('.collapsable', most_popular).slideDown();
          expanded = true;
          trigger.addClass('expanded');
        }
      });
    }
    else {
      $('h2 a.more', most_popular).hide();
    }
  }
})(jQuery);