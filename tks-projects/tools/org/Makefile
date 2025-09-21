
ORG_OPTIONS= -w

ifeq "$(DEBUG)" "1"
ORG:=tks org -d $(ORG_OPTIONS)
else
ORG:=tks org $(ORG_OPTIONS)
endif

MD:=tks app:md -il

DEPS= \
	org.tkp \
	shared.tks \
	utils.tks \
	smmx.tks \
	org.tks \
	scheduler.tks \
	smmx_output.templ

#ifeq ("$(OS)","Windows_NT")
#INSTALL_PATH:=/c/Program Files/tks/
#else
#INSTALL_PATH:=/usr/lib/tks/
#endif
INSTALL_PATH:=$(TKS_TARGET_SITE_PREFIX)


org.tkx: $(DEPS)
	tks -c org

.PHONY: install
install: org.tkx
	chmod ugo+r org.tkx
	cp org.tkx "$(INSTALL_PATH)/applications/"

.PHONY: check
check:
#	@echo $(OS)
ifeq ("$(OS)","Windows_NT")
	@echo "windows"
else
	@echo "linux"
endif
	@echo "Install-path is \"$(INSTALL_PATH)\""


#------------------------------------------------------------------------------ docs
.PHONY: doc_smmx_to_org
doc_smmx_to_org:
	cp /f/cloud/Dropbox/SimpleMind/projects/org_todo.smmx doc/todo.smmx 
	$(ORG) doc/todo.smmx -o doc/todo.org

.PHONY: doc_org_to_smmx
doc_org_to_smmx:
	$(ORG) doc/todo.org -o doc/todo.smmx
	$(ORG) doc/todo.smmx -o out_org/debug.org
	cp doc/todo.smmx /f/cloud/Dropbox/SimpleMind/projects/org_todo.smmx

.PHONY: org_manual
org_manual:
	$(MD) doc/org_manual.md

.PHONY: org_todo
org_todo:
#	$(ORG) -s -sde -afg -stt -ssg -ssgo -stls doc/todo.txt -o out_html/todo.html -now 01Jan2020
	$(ORG) -pt doc/todo.txt -o out_html/todo.html -now 01Jan2020


#------------------------------------------------------------------------------ smmx<>org tests
.PHONY: debug
debug_plan:
#	to smmx
	$(ORG) tests_org/plan.org -o out_smmx/debug.smmx

#  to org
	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

#  and back to smmx
	$(ORG) out_org/debug.org -o out_smmx/debug2.smmx

#  and back to org again
	$(ORG) out_smmx/debug2.smmx -o out_org/debug2.org

.PHONY: debug_nested_info
debug_nested_info:
#	$(ORG) tests_org/nested_info.org -o out_org/debug.org
	$(ORG) tests_org/nested_info.org -o out_smmx/debug.smmx
	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: debug_progress_list
debug_progress_list:
	$(ORG) tests_org/progress_list.org -o out_org/debug.org
#	$(ORG) tests_org/progress_list.org -o out_smmx/debug.smmx
#	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: debug_links
debug_links:
	$(ORG) tests_org/links.org -o out_org/debug.org
#	$(ORG) tests_org/links.org -o out_smmx/debug.smmx
#	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: debug_images
debug_images:
	$(ORG) tests/images.smmx -o out_org/debug.org
	$(ORG) out_org/debug.org -o out_smmx/debug.smmx

.PHONY: debug_smmx
debug_smmx:
	$(ORG) tests/smmx.smmx -o out_org/debug.org
	$(ORG) out_org/debug.org -o out_smmx/debug.smmx

.PHONY: debug_propertyname_title
debug_propertyname_title:
	$(ORG) tests_org/propertyname_title.org -o out_smmx/debug.smmx
	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: test_relations
test_relations:
	$(ORG) "tests/relations.smmx" -o "out_org/relations.org"
	$(ORG) tests/relations.smmx -o out_md/relations.md
	$(MD) out_md/relations.md
	$(ORG) out_org/relations.org -o out_smmx/debug.smmx

.PHONY: test_annotated_relations
test_annotated_relations:
	$(ORG) "tests/annotated relations.smmx" -o "out_org/annotated relations.org"
	$(ORG) "tests/annotated relations.smmx" -o "out_md/annotated relations.md"
	$(MD) "out_md/annotated relations.md"
	$(ORG) "out_org/annotated relations.org" -o out_smmx/debug.smmx

.PHONY: debug_textfmt
debug_textfmt:
	$(ORG) tests/text_formatting.smmx -o out_org/debug.org
	$(ORG) out_org/debug.org -o out_smmx/debug.smmx

.PHONY: debug_smmx_todo
debug_smmx_todo:
	$(ORG) tests_org/smmx.org -o out_smmx/todo.smmx
	$(ORG) -g 2 out_smmx/todo.smmx -o out_org/todo.org

.PHONY: debug_smmx_todo2
debug_smmx_todo2:
#	cp /f/cloud/Dropbox/SimpleMind/projects/smmx.smmx tests/smmx.smmx
	$(ORG) tests/smmx.smmx     -o out_org/smmx.org
##	$(ORG) out_org/smmx.org    -o out_smmx/todo.smmx
	$(ORG) -g 1 tests_org/smmx.org  -o out_org/debug2.org
	$(ORG) tests_org/smmx.org  -o out_smmx/todo.smmx
	$(ORG) out_smmx/todo.smmx  -o out_org/todo.org
	$(ORG) out_org/todo.org    -o out_smmx/debug.smmx
	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: debug_simple
debug_simple:
	$(ORG) -t 1 tests_org/simple.org -o out_org/debug.org

.PHONY: debug_simple_smmx
debug_simple_smmx:
	$(ORG) tests_org/simple.org -o out_smmx/simple.smmx
	$(ORG) -t 1 out_smmx/simple.smmx -o out_org/debug.org

.PHONY: debug_utf8_umlaut
debug_utf8_umlaut:
	$(ORG) tests/utf8_umlaut.smmx -o out_org/utf8_umlaut.org
	$(ORG) out_org/utf8_umlaut.org -o out_smmx/debug.smmx
	$(ORG) out_smmx/debug.smmx -o out_org/debug.org

.PHONY: debug_utf8_raw
debug_utf8_raw:
	$(ORG) -raw tests/utf8_raw.smmx -o out_org/utf8_raw.org
	$(ORG) -raw out_org/utf8_raw.org -o out_smmx/debug.smmx
	$(ORG) -raw out_smmx/debug.smmx -o out_org/debug.org



#------------------------------------------------------------------------------ schedule test
.PHONY: debug_schedule
debug_schedule:
	$(ORG) -s tests_org/debug.org

.PHONY: debug_schedule_simple
debug_schedule_simple:
	$(ORG) -s tests_org/schedule_simple.org -o out_md/schedule_simple.md

.PHONY: debug_schedule_simple2
debug_schedule_simple2:
	$(ORG) -s tests_org/schedule_simple2.org -o out_md/schedule_simple2.md

.PHONY: debug_schedule_simple3
debug_schedule_simple3:
	$(ORG) -s tests_org/schedule_simple3.org -o out_md/schedule_simple3.md

.PHONY: debug_schedule_simple4
debug_schedule_simple4:
	$(ORG) -s tests_org/schedule_simple4.org -o out_md/schedule_simple4.md

.PHONY: debug_schedule_simple5
debug_schedule_simple5:
	$(ORG) -s tests_org/schedule_simple5.org -o out_csv/schedule_simple5.csv
	$(ORG) -s tests_org/schedule_simple5.org -o out_md/schedule_simple5.md


.PHONY: debug_schedule_sim_city
debug_schedule_sim_city:
	$(ORG) -s tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_csv/sim_city.csv
	$(ORG) -s tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_md/sim_city.md

.PHONY: debug_schedule_sim_city_html
debug_schedule_sim_city_html:
	$(ORG) -s -sbsd -stl tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_html/sim_city.html
	$(ORG) -s -sbsd -ssg tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_html/sim_city_no_gantt.html
	$(ORG) -s -sbsd -stl -srv tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_html/sim_city_resource_view.html
	$(ORG) -s -sbsd -stl -srv tests_org/sim_city.org -ms "a-walk-in-the-park" -o out_html/sim_city_customized.html -ht html_templates/example_template.html

.PHONY: debug_schedule_sim_city_debug1
debug_schedule_sim_city_debug1:
	$(ORG) -s tests_org/sim_city_debug1.org -o out_md/sim_city_debug1.md

.PHONY: debug_schedule_sim_city_debug2
debug_schedule_sim_city_debug2:
	$(ORG) -s tests_org/sim_city_debug2.org -o out_md/sim_city_debug2.md

.PHONY: debug_schedule_sim_city_debug3
debug_schedule_sim_city_debug3:
	$(ORG) -s tests_org/sim_city_debug3.org -o out_md/sim_city_debug3.md

.PHONY: debug_schedule_sim_city_debug4
debug_schedule_sim_city_debug4:
	$(ORG) -s tests_org/sim_city_debug4.org -o out_md/sim_city_debug4.md

.PHONY: debug_sim_city_to_org
debug_sim_city_to_org:
# (note) use org-style node headers for a nesting depth of up to 10 (i.e. all nodes)
# (note) place tags last (org-mode style)
	$(ORG) -tl -g 10 tests_org/sim_city.org -o out_org/sim_city_reexport.org

.PHONY: debug_parallel_schedule
debug_parallel_schedule:
	$(ORG) -s tests_org/parallel_schedule.org -o out_md/parallel_schedule.md

.PHONY: debug_parallel_schedule_2
debug_parallel_schedule_2:
	$(ORG) -s tests_org/parallel_schedule_2.org -o out_md/parallel_schedule_2.md

.PHONY: debug_serial_schedule
debug_serial_schedule:
	$(ORG) -s tests_org/serial_schedule.org -o out_md/serial_schedule.md

.PHONY: debug_ordered_schedule
debug_ordered_schedule:
	$(ORG) -s tests_org/ordered_schedule.org -o out_md/ordered_schedule.md
	$(MD) out_md/ordered_schedule.md
	$(ORG) -s tests_org/ordered_schedule.org -o out_csv/ordered_schedule.csv
	$(ORG) -s tests_org/ordered_schedule.org -o out_html/ordered_schedule.html

.PHONY: debug_for_schedule
debug_for_schedule:
	$(ORG) -s tests_org/for_schedule.org -o out_md/for_schedule.md -ms "spring-release"
	$(MD) out_md/for_schedule.md  -ms "spring-release"
	$(ORG) -s tests_org/for_schedule.org -o out_csv/for_schedule.csv -ms "spring-release"
	$(ORG) -s tests_org/for_schedule.org -o out_html/for_schedule.html -ms "spring-release"

.PHONY: debug_include_test
debug_include_test:
	$(ORG) -s tests_org/include_test.org

.PHONY: debug_include_test_nested
debug_include_test_nested:
	$(ORG) -s tests_org/include_test_nested.org -o out_html/include_test_nested.html

.PHONY: debug_hide_test
debug_hide_test:
	$(ORG) -s tests_org/hide_test.org

.PHONY: debug_hash_tags
debug_hash_tags:
	$(ORG) -f tags -s tests_org/hash_tags.org

.PHONY: debug_private_export_test
debug_private_export_test:
	$(ORG) -psp -p tests_org/hide_test.org -o out_org/debug.org

.PHONY: debug_allowed_tags
debug_allowed_tags:
	$(ORG) -n tests_org/allowed_tags.org

.PHONY: debug_schedule_finished_before_start
debug_schedule_finished_before_start:
	$(ORG) -s tests_org/schedule_finished_before_start.org -o out_html/schedule_finished_before_start.html

.PHONY: debug_schedule_fold_microtasks
debug_schedule_fold_microtasks:
	$(ORG) -s tests_org/schedule_fold_microtasks.org -o out_html/schedule_fold_microtasks.html

.PHONY: debug_timetracking
debug_timetracking:
	$(ORG) -s tests_org/timetracking.org -o out_html/timetracking.html
#-itt 
#-now 01Jun2020

.PHONY: debug_timetracking_by_task
debug_timetracking_by_task:
	$(ORG) -s tests_org/timetracking_by_task.org -o out_html/timetracking_by_task.html

.PHONY: debug_timetracking_clock
debug_timetracking_clock:
	$(ORG) -s tests_org/timetracking_clock.org -o out_html/timetracking_clock.html

.PHONY: debug_timetracking_clock_fold
debug_timetracking_clock_fold:
	$(ORG) -s -srv tests_org/timetracking_clock_fold.org -o out_html/timetracking_clock_fold.html -now 20200305

.PHONY: debug_timetracking_path_match
debug_timetracking_path_match:
	$(ORG) -s tests_org/timetracking_path_match.org -o out_html/timetracking_path_match.html -now 10Feb2020

.PHONY: debug_timetracking_allocate_missing
debug_timetracking_allocate_missing:
	$(ORG) -s tests_org/timetracking_allocate_missing.org -o out_html/timetracking_allocate_missing.html

.PHONY: debug_timetracking_traffic_lights
debug_timetracking_traffic_lights:
	$(ORG) -s -stl tests_org/timetracking_traffic_lights.org -o out_html/timetracking_traffic_lights.html -now 10Feb2020

.PHONY: debug_public_resource
debug_public_resource:
	$(ORG) -psp -neh -ttp sim-city tests_org/private_resource.org -o out_org/public_resource.org

.PHONY: debug_schedule_partial_allocation
debug_schedule_partial_allocation:
	$(ORG) -s tests_org/partial_allocation.org -o out_html/partial_allocation.html

.PHONY: debug_tags_first_without_colon
debug_tags_first_without_colon:
	$(ORG) -psp -tl tests_org/tags_first_without_colon.org -so org

.PHONY: debug_plain_todo_plan_to_html
debug_plain_todo_plan_to_html:
	$(ORG) -s -sde -afg tests_org/plain_todo_plan.txt -o out_html/plain_todo_plan.html

.PHONY: debug_plain_todo_plan_all_done_to_html
debug_plain_todo_plan_all_done_to_html:
	$(ORG) -s -sde -afg tests_org/plain_todo_plan_all_done.txt -o out_html/plain_todo_plan_all_done.html

.PHONY: debug_escaped_textline_blocks
debug_escaped_textline_blocks:
	$(ORG) -psp tests_org/escaped_textline_blocks.org -o out_org/echo_escaped_textline_blocks.org

.PHONY: debug_markdown_docs
debug_markdown_docs:
	$(ORG) -s --docs -stl tests_org/markdown_docs.org -o out_html/markdown_docs.html -now 05Feb2020

.PHONY: debug_markdown_docs_no_plan
debug_markdown_docs_no_plan:
	$(ORG) -docs tests_org/markdown_docs.org -o out_html/markdown_docs_no_plan.html

.PHONY: debug_inlined_ids_md
debug_inlined_ids_md:
	$(ORG) -docs tests_org/inlined_ids.org -so md

.PHONY: debug_inlined_ids_html
debug_inlined_ids_html:
	$(ORG) -docs tests_org/inlined_ids.org -so html

.PHONY: debug_interrupted
debug_interrupted:
	$(ORG) -s -srv tests_org/interrupted.org -o out_html/interrupted.html -now 11Feb2020

.PHONY: debug_interrupted2
debug_interrupted2:
	$(ORG) -s -srv tests_org/interrupted2.org -o out_html/interrupted2.html -now 11Feb2020

.PHONY: debug_multi
debug_multi:
	$(ORG) -s -mp -srv tests_org/multi.org -o out_html/multi.html -now 11Feb2020

.PHONY: debug_multi_compact
debug_multi_compact:
	$(ORG) -sc -mp -srv tests_org/multi.org -o out_html/multi_compact.html -now 11Mar2020

.PHONY: debug_multi_compact_flt
debug_multi_compact_flt:
	$(ORG) -sc -mp -srv tests_org/multi.org -o out_html/multi_compact_flt.html -now 11Mar2020 -prj "project-two"
#	$(ORG) -sc -mp -srv tests_org/multi.org -o out_html/multi_compact_flt.html -now 11Mar2020 -prj "project-two" -prj "project-one"
#	$(ORG) -sc -mp -srv tests_org/multi.org -o out_html/multi_compact_flt.html -now 11Mar2020 -prj "implement"



.PHONY: debug_all
debug_all: \
	org.tkx \
	debug_plan \
	debug_nested_info \
	debug_progress_list \
	debug_links \
	debug_images \
	debug_smmx \
	debug_propertyname_title \
	debug_textfmt \
	debug_smmx_todo \
	debug_smmx_todo2 \
	debug_simple \
	debug_simple_smmx \
	debug_utf8_umlaut \
	debug_utf8_raw \
	debug_schedule \
	debug_schedule_simple \
	debug_schedule_simple2 \
	debug_schedule_simple3 \
	debug_schedule_simple4 \
	debug_schedule_simple5 \
	debug_schedule_sim_city \
	debug_schedule_sim_city_html \
	debug_schedule_sim_city_debug1 \
	debug_schedule_sim_city_debug2 \
	debug_schedule_sim_city_debug3 \
	debug_schedule_sim_city_debug4 \
	debug_sim_city_to_org \
	debug_parallel_schedule \
	debug_parallel_schedule_2 \
	debug_serial_schedule \
	debug_ordered_schedule \
	debug_for_schedule \
	debug_include_test \
	debug_include_test_nested \
	debug_hide_test \
	debug_hash_tags \
	debug_private_export_test \
	debug_allowed_tags \
	debug_schedule_finished_before_start \
	debug_schedule_fold_microtasks \
	debug_timetracking \
	debug_timetracking_by_task \
	debug_timetracking_clock \
	debug_timetracking_clock_fold \
	debug_timetracking_path_match \
	debug_timetracking_allocate_missing \
	debug_timetracking_traffic_lights \
	debug_public_resource \
	debug_schedule_partial_allocation \
	debug_tags_first_without_colon \
	debug_plain_todo_plan_to_html \
	debug_plain_todo_plan_all_done_to_html \
	debug_escaped_textline_blocks \
	debug_markdown_docs \
	debug_markdown_docs_no_plan \
	debug_inlined_ids_md \
	debug_inlined_ids_html \
	debug_interrupted \
	debug_interrupted2 \
	debug_multi \
	debug_multi_compact \
	debug_multi_compact_flt


.PHONY: all
all: debug_all org_manual org_todo
