(function() {
  var js,
    fjs = document.getElementsByTagName('script')[0],
    add = function(url, id) {
      if (document.getElementById(id)) {return;}
      js = document.createElement('script');
      js.src = url;
      id && (js.id = id);
      fjs.parentNode.insertBefore(js, fjs);
    };
    
  // Twitter SDK
  add('//platform.twitter.com/widgets.js', 'twitter-wjs');
  // Google+ button
  add('https://apis.google.com/js/plusone.js');
  // Facebook SDK
  add('//connect.facebook.net/en_US/all.js#xfbml=1&appId=108851635827823', 'facebook-jssdk');
}());

(function ($) {
	$widgets_sets = $('.node-blog-post .widgets-set, .node-article .quicktabs-tabpage .widgets-set, .node-confessions .widgets-set');
	
  // This is temporary until it is set in the gs theme stylesheet.
	$widgets_sets.css({opacity:0});
	
  // TODO: Find a better way to reveal the social media icons.
  $(document).ready(setTimeout(function() {
    $widgets_sets.animate({opacity:1}, 1500);
  }, 4000));
  
  /*Drupal.behaviors.gs_socialmedia = {
    attach: function (context, settings) {
      
      $('.widgets-set', context).once('gs_socialmedia', function () {
        var loadCount = 0;
        var d = new Date();
        
        var facebook = $('.social.fb', this);
        var twitter = $('.social.twitter', this);
        var plusone = $('.social.plusone', this);
        
        facebook.css({opacity:0});
        twitter.css({opacity:0});
        plusone.css({opacity:0});
        
        facebook.bind('DOMNodeInserted', function() {
          if ($('iframe', this).length > 0) {
            // The facebook iframe is triggering the load event more than once.
            // This causes the bar to be revealed before it's finally loaded.
            $('iframe', this).load(function(){
              facebook.animate({opacity:1}, 1500);
              console.log('Facebook loaded in: ' + (new Date() - d));
              facebook.unbind('DOMNodeInserted');
            });
          }
        });
        twitter.bind('DOMNodeInserted', function() {
          if ($('iframe', this).length > 0) {
            $('iframe', this).load(function(){
              twitter.animate({opacity:1}, 1500);
              console.log('Twitter loaded in: ' + (new Date() - d));
              twitter.unbind('DOMNodeInserted');
            });
          }
        });
        plusone.bind('DOMNodeInserted', function() {
          if ($('iframe', this).length > 0) {
            $('iframe', this).load(function(){
              plusone.animate({opacity:1}, 1500);
              console.log('Plusone loaded in: ' + (new Date() - d));
              plusone.unbind('DOMNodeInserted');
            });
          }
        });
      });
    }
  };*/
})(jQuery);