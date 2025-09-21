% Org-Mode Test Document
% smmx_to_markdown
% 11-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



Org-Mode Test Document
======================
#+smmx-style-key: system.gray-scale#+TODO: IDEA TODO ACTV TEST DONT IGNR NOTE | DONE#+STARTUP: showeverything



## Example Plan
:PROPERTIES::org-empty-line-after-node: t:cookie_data: todo recursive:END:Example work breakdown structure and effort estimation.The cookie_data property is used to force a recursive progress calculation (iterate all subtrees).



### Resources
:PROPERTIES::org-empty-line-after-node: t:END:



#### Devs
:PROPERTIES::org-empty-line-after-node: t:resource_id: devs:resource_type: group:END:



##### Peter Parker
:PROPERTIES::org-tags: :sw::org-empty-line-after-node: t:resource_id: spiderman:resource_type: male:limits: { dailymax 6.0h }:END:



##### Bruce Wayne
:PROPERTIES::org-tags: :hw::org-empty-line-after-node: t:resource_id: batman:resource_type: dev:limits: { dailymax 8.5h }:workinghours: mon - fri 22:00 - 04:00, 17:00 - 19:30:END:



##### Diana Prince
:PROPERTIES::org-tags: :sw::org-empty-line-after-node: t:resource_id: wonderwoman:resource_type: female:END:



##### Bruce Banner
    unspecified :resource_type:


:PROPERTIES::org-tags: :sw:test::org-empty-line-after-node: t:resource_id: hulk:limits: { dailymax 8.0h }:END:



##### Kamala Khan
:PROPERTIES::org-empty-line-after-node: t:resource_id: kamala:resource_type: dev:vacation: <2020-08-05 Wed> - <2020-08-28 Fri>:vacation: <2020-11-02 Mon> - <2020-11-06 Fri>:END:



#### Management
:PROPERTIES::org-empty-line-after-node: t:resource_id: mgr:resource_type: group:END:



##### Harleen Quinzel
:PROPERTIES::org-empty-line-after-node: t:resource_id: harley:resource_type: pm:END:



##### Arthur Fleck
:PROPERTIES::org-empty-line-after-node: t:resource_id: joker:resource_type: boss:END:



### Tasks
:PROPERTIES::org-empty-line-after-node: t:END:



#### Specification
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 32h:allocate: batman, spiderman, hulk:END:



#### Implementation
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### Hardware
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



###### Unit 1
:PROPERTIES::org-tags: :hw:unit::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 56h:allocate: hulk:END:



###### Unit 2
:PROPERTIES::org-tags: :hw:unit::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 96h:allocate: hulk:END:



###### Tests
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 80h:allocate: hulk:END:



##### Software
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



###### Driver
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



####### Kernel
Date: 10-1-2020

! 10Jan2020:PROPERTIES::smmx-calendar-date-type: create:effort: 9d:allocate: batman:END:



######## simplified tree header 1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



######### todo item 1.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



######### done item 1.2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



######## simplified tree header 2
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



######### todo item 2.1
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



######### done item 2.2
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



####### Userspace
Date: 07-3-2020

! F:07Mar2020:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: finish:effort: 72h:blocker: Kernel:allocate: batman:depends: kernel:END:



###### User Interface
:PROPERTIES::org-tags: :ui::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 120h:allocate: spiderman:END:



###### Tests
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 13d:allocate: spiderman:END:



#### Integration Test
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:effort: 20d:blocker: driver, user-interface:allocate: batman, spiderman, hulk:END:



#### Releases
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### alpha
Date: 20-05-2020

SCHEDULED: <2020-05-20 Wed>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### beta
Date: 14-09-2020

SCHEDULED: <2020-09-14 Mon>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### rc1
Date: 10-11-2020

SCHEDULED: <2020-11-10 Tue>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### rc2
Date: 03-12-2020

SCHEDULED: <2020-12-03 Thu>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



##### final
Date: 21-12-2020

SCHEDULED: <2020-12-21 Mon>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



## Tests
:PROPERTIES::org-empty-line-after-node: t:END:



### Calendar dates
:PROPERTIES::org-empty-line-after-node: t:END:



#### org mode style
Date: 24-12-2020

    src: <2020-12-24 Thu>


SCHEDULED: <2020-12-24 Thu>:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



#### dmmmyyyy style
Date: 24-12-2020

    src: 24Dec2020


SCHEDULED: 24Dec2020:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



#### smmx style
Date: 24-12-2020

    src: 24-12-2020


SCHEDULED: 24-12-2020:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



#### yyyymmdd style
Date: 24-12-2020

    src: 20201224


SCHEDULED: 20201224:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



### Links
:PROPERTIES::org-empty-line-after-node: t:END:



#### theuselessweb.com
**Link**: [https://theuselessweb.com/](https://theuselessweb.com/) *(url)*

    child node link is added to this node


:PROPERTIES::END:



#### link with children (parent node)
:PROPERTIES::END:



##### http://www.koalastothemax.com
**Link**: [http://www.koalastothemax.com](http://www.koalastothemax.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### the previous link is _not_ added to the parent node
    because it has a child node


:PROPERTIES::org-layout: list:END:



##### a list of rather useless links
    the first link is _not_ moved to the parent node here


:PROPERTIES::org-layout: list:END:



###### http://www.rrrgggbbb.com
**Link**: [http://www.rrrgggbbb.com](http://www.rrrgggbbb.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://endless.horse
**Link**: [http://endless.horse](http://endless.horse) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://ihasabucket.com
**Link**: [http://ihasabucket.com](http://ihasabucket.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### https://pointerpointer.com
**Link**: [https://pointerpointer.com](https://pointerpointer.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://chihuahuaspin.com
**Link**: [http://chihuahuaspin.com](http://chihuahuaspin.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://www.blankwindows.com
**Link**: [http://www.blankwindows.com](http://www.blankwindows.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://www.hackertyper.com
**Link**: [http://www.hackertyper.com](http://www.hackertyper.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://notdayoftheweek.com
**Link**: [http://notdayoftheweek.com](http://notdayoftheweek.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://pixelsfighting.com
**Link**: [http://pixelsfighting.com](http://pixelsfighting.com) *(url)*

:PROPERTIES::org-layout: list:END:



###### http://buildshruggie.com
**Link**: [http://buildshruggie.com](http://buildshruggie.com) *(url)*

:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



### Custom icons
    preserved when node has state 'none'


:PROPERTIES::org-empty-line-after-node: t:END:



#### item 2
:PROPERTIES::org-layout: list:END:



##### item 2.1
:PROPERTIES::org-layout: list:END:



###### item 2.2.1
:PROPERTIES::org-layout: list:END:



##### item 2.2
:PROPERTIES::org-layout: list:END:



###### item 2.2.2
:PROPERTIES::org-layout: list:END:



###### item 2.2.3
:PROPERTIES::org-layout: list:END:



#### item 3
:PROPERTIES::org-layout: list:END:



##### item 3.1
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



## todo.txt style structures
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



### can be mixed with org-style ** headers
:PROPERTIES::org-layout: list:END:



#### * style headers may contain todo-style headers
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



### global ! notes will be added to "info" node
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



## groceries
    a simple todo.txt style tree


:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### bread
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### butter
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### cheese
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### eggs
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### pasta
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### rice
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### fruit
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### apples
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### clementines
    october .. february


:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### bananas
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### vegetables
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### tomatoes
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### cucumber
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### onions
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### potatoes
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### carrots
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### peas
    frozen


:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### kohlrabi
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### spinach
    frozen


:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### mushrooms
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### red peppers
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



### drinks
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### water
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### tea
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### coffee
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### milk
:PROPERTIES::org-layout: list:smmx-calendar-date-type: scheduled:END:



#### juice
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



## create/mod/finish dates
:PROPERTIES::org-empty-line-after-node: t:smmx-calendar-date-type: scheduled:END:



### create date
Date: 09-1-2020

    created 09Jan2020


! 09Jan2020:PROPERTIES::smmx-calendar-date-type: create:END:



#### this is a note
:PROPERTIES::org-layout: list:END:



##### this is a nested note
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



### modify/change dates
Date: 01-1-2020

    modified 02Jan2020, 03Jan2020


! 01Jan2020! C:02Jan2020! C:03Jan2020:PROPERTIES::smmx-calendar-date-type: create:next: :END:



#### this is a note
:PROPERTIES::org-layout: list:END:



##### this is a nested note
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



### finish date
Date: 04-1-2020

    finished on 04Jan2020


! F:04Jan2020:PROPERTIES::smmx-calendar-date-type: finish:END:



#### this is a note
:PROPERTIES::org-layout: list:END:



##### this is a nested note
:PROPERTIES::org-layout: list:org-empty-line-after-node: t:END:



### create/modify/finish dates
Date: 04-1-2020

    modified on 2./3. and finished on 04Jan2020


! 01Jan2020! C:02Jan2020! C:03Jan2020! F:04Jan2020:PROPERTIES::smmx-calendar-date-type: finish:END:



#### this is a note
:PROPERTIES::org-layout: list:END:



##### this is a nested note
:PROPERTIES::org-layout: list:END:



info
====
. ##+smmx-style-key: system.compact-gray-scale! https://orgmode.org/manual/Initial-visibility.html! unfold all nodes but keep properties folded:! #+STARTUP: showall! unfold all nodes and properties:! set org-export-headline-levels via #+OPTIONS: H:4! https://orgmode.org/manual/index.html! https://orgmode.org/manual/Property-Syntax.html#Property-Syntax   ! "Keys are case-insensitive."! https://orgmode.org/manual/Special-Properties.html#Special-Properties! properties   ! C-c C-x e   add effort property   ! C-c C-x p   add property (prompt for name+value)   ! C-c C-c d   delete property! date stamps:   C-c .   show calendar   C-c C-d (org-deadline)   C-c C-s (org-schedule)! progress   C-C C-c  update progress [30%] or [2/3]! outline-toggle-children   fold/unfold direct children (not recursively)! outline-show-subtree   unfold entire subtree (including properties)! outline-show-all   unfold all subtrees (including properties)! http://taskjuggler.org/tj3/manual/index.html   ! http://taskjuggler.org/tj3/manual/dailymax.html! org-mode compatibility   ! element order      ! SCHEDULED:      ! DEADLINE:      !                                                                  :properties:          > aka drawers      ! ctl lines         > create, modify, finish dates      ! > callout lines      ! ! <link>      ! any other text lines   ! app:org limitations      ! no nested drawers   ! app:org extensions      ! arbitrary element orders         > fixed to org-mode order when writing .org files      ! todo.txt style trees         ! including properties      ! ctl lines         ! start with "! "         ! create, modify, finish dates            ! "! 11Jan2020" (create)            ! "! C:12Jan2020" (modify/change)               ! may appear multiple times            ! "! F:13Jan2020" (finish)         ! link      ! callouts         ! text callout via "> my callout text line"            > may appear multiple times         ! image callout via "> images/myimage.png"            > may only appear once      ! info nodes before first task node         > e.g. for research notes      ! support for taskjuggler properties         ! effort         ! resource_id         ! allocated         ! blocker         ! depends         ! limits            > workhours         ! extensions            ! resource_type               > dev,group,pm, ..            ! vacation      ! preserves mindmap UI settings via "smmx-" properties         ! positions, text styles, shape styles, font sizes, colors, ..



$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
