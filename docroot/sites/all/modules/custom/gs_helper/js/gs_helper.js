(function ($) {
  
  Drupal.behaviors.gs_helper = {
    attach: function (context, settings) {
      
    }
  };
  
  // Override the tabs behaviour to apply tab triggering to our custom lead-in blocks, too
  if (Drupal.quicktabs) {
    Drupal.quicktabs.prepare = function(el) {
      var photos_block = $('.photos_lead_in-block');
      $('a', photos_block).attr('href',$('a', '.tab-photos').attr('href'));
      var videos_block = $('.videos_lead_in-block');
      $('a', videos_block).attr('href',$('a', '.tab-videos').attr('href'));
      
      // el.id format: "quicktabs-$name"
      var qt_name = Drupal.quicktabs.getQTName(el);
      var $ul = $(el).find('ul.quicktabs-tabs:first');
      $ul.find('li a').each(function(i, element){
        element.myTabIndex = i;
        element.qt_name = qt_name;
        var tab = new Drupal.quicktabs.tab(element);
        var parent_li = $(element).parents('li').get(0);
        if ($(parent_li).hasClass('active')) {
          $(element).addClass('quicktabs-loaded');
        }
        $(element).once(function() {
          $(this).bind('click', {tab: tab}, Drupal.quicktabs.clickHandler);
          // photos
          // if (i == 1 && photos_block.length) {
          if ($(parent_li).hasClass("tab-photos") && photos_block.length) {
	        console.log('photos');
            var thumb = $('a', photos_block);
            var link  = $('a', photos_block);
            Drupal.quicktabs_gs.leadinPrepare(tab, this, thumb, link);
          }
          // videos
          // if (i == 2 && videos_block.length) {
          if ($(parent_li).hasClass("tab-videos") && videos_block.length) {
	        console.log('videos');
            var thumb = $('a', videos_block);
            var link  = $('a', videos_block);
	        Drupal.quicktabs_gs.leadinPrepare(tab, this, thumb, link);
          }
        });
      });
    }
    
    // Override the tabs behaviour to allow specifying the trigger element
    Drupal.quicktabs.clickHandler = function(event) {
      var tab = event.data.tab;
      var element = this;
      
      if (event.data.trigger) {
        element = event.data.trigger;
      }
      
      // Set clicked tab to active.
      $(element).parents('li').siblings().removeClass('active');
      $(element).parents('li').addClass('active');
    
      // Hide all tabpages.
      tab.container.children().addClass('quicktabs-hide');
      
      if (!tab.tabpage.hasClass("quicktabs-tabpage")) {
        tab = new Drupal.quicktabs.tab(element);
      }
    
      tab.tabpage.removeClass('quicktabs-hide');
      return false;
    }
    
    Drupal.quicktabs_gs = {};
    Drupal.quicktabs_gs.leadinPrepare = function(tab, trigger, thumb, link) {
      if (thumb.length) {
        $(thumb).bind('click', {tab: tab, trigger: trigger}, Drupal.quicktabs.clickHandler);
      }
      
      if (link.length) {
        $(link).bind('click', {tab: tab, trigger: trigger}, Drupal.quicktabs.clickHandler);
      }
    }
  }
  
})(jQuery);