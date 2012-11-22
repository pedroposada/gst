(function($) {
	$(document).ready(function() {
	// alert('ya man');
		var wrapperId = Drupal.settings.gs_classifieds.collapsibleTrayId;
		var innerId = Drupal.settings.gs_classifieds.collapsibleTrayInnerId;
		var moreText = Drupal.settings.gs_classifieds.collapsibleTrayMoreText;
		var lessText = Drupal.settings.gs_classifieds.collapsibleTrayLessText;
		var wrapper = $("#" + wrapperId);
		var expanded = $("#" + innerId, wrapper).height();
		var collapsed = wrapper.height();
		
		// check if there is need to expand
		if (expanded > 24) { // height of one row of topics
			$("a.more", wrapper).bind("click", function() {
				if($(this).hasClass("expanded")) {
					// collapse
					wrapper.animate({
						height : collapsed + "px",
					}, {
						duration : "fast",
						complete : function() {
							$("a.more", wrapper).removeClass("expanded");
							$("a.more", wrapper).addClass("collapsed");
							$("a.more", wrapper).text(moreText);
						},
					});
				} else {
					// expand
					wrapper.animate({
						height : expanded + "px",
					}, {
						duration : "fast",
						complete : function() {
							$("a.more", wrapper).removeClass("collapsed");
							$("a.more", wrapper).addClass("expanded");
							$("a.more", wrapper).text(lessText);
						},
					});
				}
				console.log("more link clicked.");
	
				return false;
			});
		} else {
			$("a.more", wrapper).remove();
		}
		
		console.log("topcis tray.");
		console.log("expanded: " + expanded);
		console.log("collapsed: " + collapsed);
	});
})(jQuery); 