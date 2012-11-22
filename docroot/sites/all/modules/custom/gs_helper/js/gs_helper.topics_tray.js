(function($) {
  $(document).ready(function() {
    var linkWrapperId = Drupal.settings.gs_helper.linkWrapperId;
    var wrapperId = Drupal.settings.gs_helper.collapsibleTrayId;
    var innerId = Drupal.settings.gs_helper.collapsibleTrayInnerId;
    var moreText = Drupal.settings.gs_helper.collapsibleTrayMoreText;
    var lessText = Drupal.settings.gs_helper.collapsibleTrayLessText;
    var linkWrapper = $("#" + linkWrapperId);
    var wrapper = $("#" + wrapperId);
    var expanded = $("#" + innerId, wrapper).height();
    var collapsed = wrapper.height();
    
    // check if there is need to expand
    if (expanded > 24) { // height of one row of topics
      $("a.more", linkWrapper).bind("click", function() {
        if($(this).hasClass("expanded")) {
          // collapse
          wrapper.animate({
            height : collapsed + "px",
          }, {
            duration : "fast",
            complete : function() {
              $("a.more", linkWrapper).removeClass("expanded");
              $("a.more", linkWrapper).addClass("collapsed");
              $("a.more", linkWrapper).text(moreText);
            },
          });
        } else {
          // expand
          wrapper.animate({
            height : expanded + "px",
          }, {
            duration : "fast",
            complete : function() {
              $("a.more", linkWrapper).removeClass("collapsed");
              $("a.more", linkWrapper).addClass("expanded");
              $("a.more", linkWrapper).text(lessText);
            },
          });
        }
        console.log("more link clicked.");
        return false;
      });
    } else {
      $("a.more", linkWrapper).remove();
    }
  });
})(jQuery); 