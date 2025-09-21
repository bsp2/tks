% Staff
% smmx_to_markdown
% 22-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



Staff
=====
#+PROJECT-START: <2020-02-03 Mon>



## Staff
    Everything rests upon the shoulders of these three people


:PROPERTIES::org-separator-style: equal:org-separator-caption: Staff:END:



### Peter
:PROPERTIES::resource_id: peter:workinghours: mo-thu 6h, fr off:phone-office: 87:vacation: <2020-01-28 Tue> - <2020-02-14 Fri>:vacation: <2020-07-09 Thu> - <2020-07-10 Fri>:END:



### Paul
:PROPERTIES::resource_id: paul:workinghours: mo-fr 9-5:phone-office: 19:vacation: <2020-02-24 Mon> - <2020-03-13 Fri>:vacation: <2020-05-11 Mon> - <2020-05-13 Wed>:END:



### Mary
:PROPERTIES::org-empty-line-after-node: t:resource_id: mary:workinghours: mo-tue 8h, wed off, thu-fr 6.5h:phone-office: 28:vacation: <2020-03-04 Wed> - <2020-03-20 Fri>:vacation: <2020-06-01 Mon> - <2020-06-12 Fri>:END:



## Main Tasks
:PROPERTIES::END:



### Task 1
:PROPERTIES::org-fold: t:org-empty-line-after-node: t:allocate: peter:effort: 10d:fold: t:END:; 'fold' means that the child nodes will not appear as separate tasks in the schedule;         but their efforts will be summed and added to the parent task effort



#### Task 1.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 1.1.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 2d:END:



##### Task 1.1.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 4d:END:



#### Task 1.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 1.2.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 2d:END:



##### Task 1.2.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:END:



### Task 2
:PROPERTIES::org-fold: t:org-empty-line-after-node: t:allocate: paul:effort: 12d:fold: t:END:; 'fold' means that the child nodes will not appear as separate tasks in the schedule;         but their efforts will be summed and added to the parent task effort



#### Task 2.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 2.1.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:END:



##### Task 2.1.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 2d:END:



##### Task 2.1.3
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 1d:END:



#### Task 2.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 2.2.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 5d:END:



##### Task 2.2.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 7d:END:



### Task 3
:PROPERTIES::org-private: t:org-empty-line-after-node: t:allocate: mary:effort: 14d:private: t:END:; 'private' means that the child nodes will not appear as separate tasks in the schedule;           plus they will not be exported to other .org files;           but their efforts will still be summed and added to the parent task effort



#### Task 3.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 3.1.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 4d:END:



##### Task 3.1.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 6d:END:



##### Task 3.1.3
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:END:



#### Task 3.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



##### Task 3.2.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 2d:END:



##### Task 3.2.2
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 9d:END:



## Milestones
:PROPERTIES::END:



### Finish
:PROPERTIES::milestone: :depends: main-tasks:END:



info
====
. ; this is an include file. see include_test.org, hide_test.org



$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
