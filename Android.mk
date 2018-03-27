# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := protocol
LOCAL_SRC_FILES := \
	PROTOCOL/command/command.c\
	PROTOCOL/formula/formula.c\
	PROTOCOL/formula/formula_comply.c\
	PROTOCOL/formula_parse/interface.c\
	PROTOCOL/formula_parse/Parser.c\
	PROTOCOL/function/active_ecu_lib.c\
	PROTOCOL/function/actuator_test_lib.c \
	PROTOCOL/function/clear_dtc_lib.c \
	PROTOCOL/function/ds_lib.c \
	PROTOCOL/function/dtc_lib.c \
	PROTOCOL/function/freeze_lib.c \
	PROTOCOL/function/idle_link_lib.c \
	PROTOCOL/function/infor_lib.c \
	PROTOCOL/function/quit_system_lib.c \
	PROTOCOL/function/special_function_lib.c \
	PROTOCOL/function/scan_ecu_version.c \
	PROTOCOL/InitConfigFromXml/init_active_ecu_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_config_from_xml_lib.c \
	PROTOCOL/InitConfigFromXml/init_dtc_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_freeze_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_freeze_ds_formula_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_idle_link_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_information_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_process_fun_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_protocol_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_security_access_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_special_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_specific_command_config_lib.c \
	PROTOCOL/InitConfigFromXml/init_vci_config_lib.c \
	PROTOCOL/interface/protocol_interface.c \
	PROTOCOL/protocol/iso_15765.c \
	PROTOCOL/protocol/iso_14230.c \
	PROTOCOL/protocol/sae_1939.c \
	PROTOCOL/public/protocol_config.c \
	PROTOCOL/public/public.c \
	PROTOCOL/SpecialFunction/general_function.c \
	PROTOCOL/SpecialFunction/special_variable.c \
	PROTOCOL/SpecialFunction/specific_function.c

#LOCAL_MODULE    := main
#LOCAL_SRC_FILES := main_jni.c

#LOCAL_MODULE    := serial_port
#LOCAL_SRC_FILES := SerialPort.c

LOCAL_SHARED_LIBRARIES := XXX
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)