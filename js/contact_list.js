(function ($, Drupal, window, document, undefined) {


// To understand behaviors, see https://drupal.org/node/756722#behaviors
  Drupal.behaviors.contact_list = {
    attach: function(context, settings) {
        /** Open Close Contact List **/
        $('.contact-list-title-wrapper', context).click(function(event) {
          if ($('.contact-list-courses-groups-wrapper').hasClass('show')) {
            // Contact List is already opened so close it.
            // First close any individual course/class wrapper.
            // For accordion effect, loop through and close any that have show class.
            closeCourseGroupUsersWrappers();
            // Loop through and replace any down arrows with right arrows.
            removeDownArrows();
            // Close the contact list.
            $('.contact-list-courses-groups-wrapper').removeClass('show');
            $('.contact-list-arrow-svg-wrapper').removeClass('open');
            $('#contact-list-wrapper').removeClass('show');
            $('.contact-list-online-status-select').removeClass('show');
            //Drupal.detachBehaviors();
          }
          else {
            // Contact List is closed so open it.
            // Give the courses and groups wrapper a height so it's visible.
            $('.contact-list-courses-groups-wrapper').addClass('show');
            $('.contact-list-arrow-svg-wrapper').addClass('open');
            $('#contact-list-wrapper').addClass('show');
            $('.contact-list-online-status-select').addClass('show');
          }
          event.preventDefault();
        });

        /** Accordion effect to open contact list users in a course/group **/
        $('.course-group-accordion', context).click(function(event) {
          var gid = $(this).attr('id').replace('course-group-accordion-', '');
          var each_course_group_users = $('#each-course-group-users-wrapper-' + gid).html();
          // If users for course not loaded yet then run ajax to get them.
          if (each_course_group_users == '') {
            var url = '/contact-list/ajax/get_users_course_group';
            $.ajax({
              type: "POST",
              url: url,
              data: {
                'gid': gid
              },
              dataType: 'json',
              beforeSend: function () {
                $('#each-course-group-users-wrapper-' + gid).append('<div class="contact-list-load-users-wait">Loading people...</div>');
              },
              complete: function () {
                $('#each-course-group-users-wrapper-' + gid).append('<div class="contact-list-load-users-wait"></div>');
              },
              success: function (data) {
                if (data.user_content) {
                  $('#each-course-group-users-wrapper-' + data.gid).html(data.user_content);
                  // get correct picture for user.
                  var courseGroupContentElement = $('#each-course-group-users-wrapper-' + data.gid);
                  if ( courseGroupContentElement.children().length > 0 ) {
                    courseGroupContentElement.find('img').each(function() {
                      if ($(this).attr('alt') != $(this).attr('title')) {
                        var thisAlt = $(this).attr('alt');
                        var thisTitle = $(this).attr('title');
                        $(this).attr('src', thisAlt).attr('alt', thisTitle);
                      }
                    });
                  }
                  Drupal.attachBehaviors($('#each-course-group-users-wrapper-' + data.gid));
                  Drupal.attachBehaviors($('#contact-list-user-status-select-wrapper'));
                }
                else {
                  $('#each-course-group-users-wrapper-' + data.gid).html('<div class="nocontacts"><p>No Contacts</p></div>');
                }
              }
            });
          }
          // Open the one that's clicked.
          if ($('#each-course-group-users-wrapper-' + gid).hasClass('show')) {
            // Already opened so click causes it to close.
            $('#each-course-group-users-wrapper-' + gid).removeClass('show');
            $('#course-group-accordion-' + gid).find('.contact-list-course-group-arrow').removeClass('show');
            // Remove active on clicked course-group-accordion class to revert background color.
            $(this).removeClass('active');
            //Drupal.detachBehaviors($('#each-course-group-users-wrapper-' + gid));
          }
          else {
            // For accordion effect, loop through and close any that have show class.
            closeCourseGroupUsersWrappers();
            // And remove their down arrows.
            removeDownArrows();
            // Open the the clicked users wrapper.
            $('#each-course-group-users-wrapper-' + gid).addClass('show');
            $('#course-group-accordion-' + gid).find('.contact-list-course-group-arrow').addClass('show');
            // Add active class to course-group-accordion class to change background color to hover color.
            $(this).addClass('active');
          }
        });
    }
  };

  /** Functions **/
  // Close all wrappers for users in a course/group.
  function closeCourseGroupUsersWrappers() {
    var eachCourseGroupUsersWrapper = document.getElementsByClassName('each-course-group-users-wrapper');
    for (var i = 0; i < eachCourseGroupUsersWrapper.length; i++) {
      var el = eachCourseGroupUsersWrapper[i];
      el.className = el.className.replace('show', '');
    }
  }

  // Remove all down arrows for a course/group.
  function removeDownArrows() {
    var courseGroupAccordion = document.getElementsByClassName('course-group-accordion');
    for (var i = 0; i < courseGroupAccordion.length; i++) {
      var downArrow = document.getElementsByClassName('contact-list-course-group-arrow');
      var el = downArrow[i];
      el.className = el.className.replace('show', '');
      var activeClass = courseGroupAccordion[i];
      // While you're in here remove active class that changes background color to hover background color.
      activeClass.className = activeClass.className.replace('active')
    }
  }

})(jQuery, Drupal, this, this.document);