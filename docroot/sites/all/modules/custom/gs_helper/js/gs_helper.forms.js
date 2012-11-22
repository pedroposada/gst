(function($) {
  Drupal.behaviors.gs_helper_forms = {
	attach: function (context, settings) {
		$('form', context).once('gsForms', function () {
			// HIDE FORMATTING OPTIONS
			$(".filter-wrapper", this).hide();
		});
	}
  }
})(jQuery);