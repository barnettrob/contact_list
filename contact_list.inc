<?php

use Drupal\course_group\Data\CoursesAndGroups;
use Drupal\course_group\Data\GroupRole;
use Drupal\course_group\Data\GroupParticipants;
use Drupal\babson_profile\Data\bcard;

/** Contact list wrapper
 *
 * Gets called in hook_block_view.
 */
function contact_list_wrapper() {

  $contact_list_arrow =
    '<svg class="contact-list-arrow-svg" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="Layer_1" x="0px" y="0px" width="20px" height="20px" viewBox="0 0 20 20" enable-background="new 0 0 20 20" xml:space="preserve">
      <circle fill="none" stroke="#C6D3D7" stroke-width="2" cx="10" cy="10" r="8.5"/>
      <line class ="horiz-line" fill="none" stroke="#C6D3D7" stroke-width="3" x1="5.7" y1="10" x2="14.2" y2="10"/>
      <polygon points="10,5 13,5 6.7,10 13,15.5 10,15.5 3.7,10" fill="#C6D3D7" class="point-left" />
    </svg>';


  if (module_exists('babson_online_status')) {
    module_load_include('inc', 'babson_online_status', 'babson_online_status');
    $online_status_select = babson_online_status_selectbox();
  }
  else {
    $online_status_select = '';
  }
  $content = '<div id="contact-list-wrapper">';
  $content .=   '<div class="contact-list-title-wrapper">';
  $content .=     '<div class="contact-list-title">' . t('Contact List') . '</div>';
  $content .=     '<div class="contact-list-open-close-wrapper">';
  $content .=       '<div class="contact-list-arrow-svg-wrapper">';
  $content .=         $contact_list_arrow;
  $content .=       '</div>';
  $content .=     '</div>';
  $content .=   '</div>'; // End .contact-list-title-wrapper
  $content .= $online_status_select;
  $content .=   contact_list_courses_groups();

  return $content;
}

/**
 * Callback for contact list courses and groups for user.
 *
 * Gets called in contact_list_wrapper().
 */
function contact_list_courses_groups() {
  if (module_exists('course_group')) {
    global $user;

    $pdo = TRUE;
    $coursesAndGroups = new CoursesAndGroups($user->name, $pdo);
    $my_courses_groups = $coursesAndGroups->courses_groups_sql();

    $content =   '<div id="contact-list-courses-groups-wrapper" class="contact-list-courses-groups-wrapper">';
    $content .=     '<div class="contact-list-courses-groups-border">';
    foreach ($my_courses_groups as $course_group) {
      $content .=     '<div class="course-group-accordion" id="course-group-accordion-' . $course_group['nid'] . '">';
      $content .=       '<div class="contact-list-course-group-title">' . $course_group['title'] . '</div>';
      $content .=       '<div class="contact-list-course-group-arrow"></div>';
      $content .=     '</div>'; // End .course-group-accordion
      $content .= '<div class="each-course-group-users-wrapper" id="each-course-group-users-wrapper-' . $course_group['nid'] . '">';
      // Ajax call for users on course/group gets loaded here.
      $content .= '</div>'; // End .each-course-group-users-wrapper
    }
    // If no courses or groups then create empty div container for spacing.
    if (empty($my_courses_groups)) {
      $content .= '<div class="contact-list-course-group-empty"></div>';
    }
    $content .=     '</div>'; // End .contact-list-courses-groups-border
    $content .=   '</div>'; // End .contact-list-courses-groups-wrapper
    $content .= '</div>'; // End #contact-list-wrapper
  }
  else {
    $content = t('course_group module needs to be enabled.');
  }

  return $content;
}

/**
 * Ajax callback to load users for course or group in contact list.
 */
function ajax_contact_list_get_users_course_group() {
  global $user;

  $servername = $_SERVER['HTTP_HOST'];
  if (module_exists('babson_ribbon_menu')) {
    $switcherurl = babson_ribbon_bell_url();
  }
  else {
    global $base_url;
    $switcherurl = $base_url;
  }

  $default_url = "public://pictures/picture-691.gif";
  $switcher_default_full_url = $switcherurl . '/sites/default/files/styles/25x30/public/pictures/picture-691.gif';

  $content = '';
  if (module_exists('course_group')) {
    $gid = $_POST['gid'];
    $pdo = TRUE;
    $members_class = new GroupParticipants($gid, $pdo);
    $members = $members_class->get_participants_sql();

    // Role names.
    $group_roles_class = new GroupRole($gid, $pdo);
    $group_roles_class->group_roles();
    $admin_rid = !empty($group_roles_class->groupRoles['administrator member'][$gid]) ? $group_roles_class->groupRoles['administrator member'][$gid][0] : $group_roles_class->groupRoles['administrator member'][0][0];
    $ta_rid = !empty($group_roles_class->groupRoles['ta'][$gid]) ? $group_roles_class->groupRoles['ta'][$gid][0] : $group_roles_class->groupRoles['ta'][0][0];
    $ta_showuser_rid = !empty($group_roles_class->groupRoles['ta_showuser'][$gid]) ? $group_roles_class->groupRoles['ta_showuser'][$gid][0] : $group_roles_class->groupRoles['ta_showuser'][0][0];

    foreach ($members as $member) {
      // Don't show yourself.
      if ($member['name'] != $user->name) {
        switch($member['rid']) {
          case $admin_rid:
            $role = '';
            break;
          case $ta_rid:
            $role = t('hidden_ta');
            break;
          case $ta_showuser_rid:
            $role = '<span class="ta-badge">TA</span>';
            break;
          default:
            $role = '';
        }

        $fullname = $member['first'] . ' ' . $member['last'];

        // Communications icons.
        $email_button = '';
        $hangout_button = '';
        $droogle_button = '';
        if (module_exists('droogle_hangout')) {
          $email_button = theme('droogle_hangout_email_button', array(
            'email' => $member['mail'],
            'text' => FALSE
          ));
        }
        if (module_exists('droogle_drive')) {
          $droogle_button = theme('droogle_drive_create_invite_button', array(
            'email' => $member['mail'],
            'fullname' => $fullname,
            'text' => FALSE
          ));
          $hangout_button = theme('droogle_hangout_invite_button', array(
            'email' => $member['mail'],
            'fullname' => $fullname,
            'text' => FALSE
          ));
        }

        // Show users in list.
        $show_participants = FALSE;
        // If social group then show all participants
        if ($group_roles_class->node_type() == 'non_course_groups') {
          $show_participants = TRUE;
        }
        elseif ($group_roles_class->node_type() == 'group' && $role != 'hidden_ta') {
          // If course then only show participants who are not hidden ta.
          $show_participants = TRUE;
        }

        if ($show_participants) {

          /** Get user's picture **/
          if (isset($member['picture']) && $member['picture'] != "") {
            if (module_exists('db_pdo_switch')) {
              /*** Connect to Bell database. ***/
              $connection = db_pdo_bell_switch();
              $user_pic_filename = $connection->prepare("SELECT filename FROM file_managed WHERE fid = :fid");
              $user_pic_filename->execute(array('fid' => $member['picture']));
              $user_pic_filename = $user_pic_filename->fetchColumn(0);
            }
            if (stripos($servername,"bell")===FALSE) {
              $onerror = 'this.src=\'' . $switcher_default_full_url . '\'';
              $picture = '<img src="' . $switcher_default_full_url . '" alt="' . $switcherurl . '/sites/default/files/styles/25x30/public/pictures/' . $user_pic_filename .  '" title="' . $fullname . '"';
              if (!user_access('skip onerror')) {
                $picture .= ' onerror="this.onerror=null;' . $onerror . ';"';
              }
              $picture .= '>';
            } else {
              $user_pic_uri = "public://pictures/" . $user_pic_filename;
              $onerror = 'this.src=\'' . image_style_url("25x30", $default_url) . '\'';
              $picture = '<img src="'. image_style_url("25x30", $default_url) . '"alt="' . image_style_url("25x30", $user_pic_uri) . '" title="' . $fullname . '"';
              if (!user_access('skip onerror')) {
                $picture .= ' onerror="this.onerror=null;' . $onerror . ';"';
              }
              $picture .= '>';
            }
          }
          /** end get user's picture **/

          if (module_exists('babson_online_status')) {
            module_load_include('inc', 'babson_online_status', 'babson_online_status');
            $online_status_bubble = babson_online_status_bubble_fill($member['name']);
          }
          else {
            $online_status_bubble = '';
          }

          $content .= '<div class="contact-list-user-wrapper">';
          $content .=   '<div class="contact-list-userpic-name-role-wrapper">';
          $content .=     '<div class="roster-picture roster-element">';
          $content .=       l($picture, 'user/' . $member['uid'], array('html' => TRUE, 'attributes' => array('class' => $member['uid,'], 'title' => $fullname . '\'s profile')));
          $content .=     '</div>';
          // Only show role badge if course.
          $content .=       $group_roles_class->node_type() == 'group' ? $role : '';
          $content .=     '<div class="contact-list-user-fullname">';
          $content .=       l($fullname, 'user/' . $member['uid']);
          $content .=     '</div>';
          $content .=     '<div class="contact-list-status-svg">' . $online_status_bubble . '</div>';
          $content .=   '</div>'; // End .contact-list-userpic-name-role-wrapper
          $content .=   '<div class="contact-list-communication-wrapper">';
          $content .=     $email_button . ' ' . $hangout_button . ' '  . $droogle_button;
          $content .=   '</div>';
          $content .= '</div>';
        }
      }
    }

    drupal_json_output(array(
      'user_content' => $content,
      'gid' => $gid,
    ));
  }
  else {
    drupal_json_output(array(
      'user_content' => 'course_group module is disabled.',
    ));
  }
}