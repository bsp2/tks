% Staff
% smmx_to_markdown
% 22-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



Staff
=====
#+STARTUP: showeverything#+PROJECT-START: <2020-02-03 Mon>



## Staff
    Everything rests upon the shoulders of these three people


:PROPERTIES::org-separator-style: equal:org-separator-caption: Staff:END:



### Peter
:PROPERTIES::resource_id: peter:workinghours: mo-thu 6h, fr off:phone-office: 87:vacation: <2020-01-28 Tue> - <2020-02-14 Fri>:vacation: <2020-07-09 Thu> - <2020-07-10 Fri>:END:



### Paul
:PROPERTIES::resource_id: paul:workinghours: mo-fr 9-5:phone-office: 19:vacation: <2020-02-24 Mon> - <2020-03-13 Fri>:vacation: <2020-05-11 Mon> - <2020-05-13 Wed>:END:



### Mary
:PROPERTIES::org-empty-line-after-node: t:resource_id: mary:workinghours: mo-tue 8h, wed off, thu-fr 6.5h:phone-office: 28:vacation: <2020-03-04 Wed> - <2020-03-20 Fri>:vacation: <2020-06-01 Mon> - <2020-06-12 Fri>:END:



## Build a house
:PROPERTIES::ordered: :END:



### buy property
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 1d:allocate: peter:END:



### foundation
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 2d:allocate: paul:END:



### rough framing
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:allocate: mary:END:



### plumbing
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 4d:allocate: paul:END:



### electrical installation
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 5d:allocate: mary:END:



### insulation
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:; the following two tasks can be done in parallel



#### walls
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:allocate: peter:END:



#### roof
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 3d:allocate: mary:END:



### finish interior
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:effort: 7d:allocate: paul:END:



## Milestones
:PROPERTIES::END:



### Move in
:PROPERTIES::milestone: t:depends: build-a-house:END:



info
====
. ; this is an include file. see include_test.org, hide_test.org



$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
