(function ($) {
  if ($.browser.msie) {
    $(document).ready(setTimeout(function() {
      $('.gam-holder').each(function(){
        var size = $(this).children().size();
        
        if (size > 3) {
          // Weird case where a google ad get rendered outside of it's own wrapper.
          // Check and reposition these ads back into the appropriate element.
          $(this).children().eq(3).appendTo($(this).find('div[id$=_ad_container]'));
          $(this).children().eq(3).appendTo($(this).find('object'));
        }
      });
    }, 0));
  }
})(jQuery);