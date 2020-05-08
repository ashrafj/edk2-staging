/** @file
  This file declares EFI PCI Express Platform Protocol that provide the interface between
  the PCI bus driver/PCI Host Bridge Resource Allocation driver and a platform-specific
  driver to describe the unique features of a platform.
  This protocol is optional.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent


**/

#ifndef _PCI_EXPRESS_PLATFORM_H_
#define _PCI_EXPRESS_PLATFORM_H_

///
/// This file must be included because the EFI_PCI_EXPRESS_PLATFORM_PROTOCOL uses
/// EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE.
///
#include <Protocol/PciHostBridgeResourceAllocation.h>

///
/// Global ID for the  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL.
///
#define  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL_GUID \
  { \
    0x787b0367, 0xa945, 0x4d60, {0x8d, 0x34, 0xb9, 0xd1, 0x88, 0xd2, 0xd0, 0xb6} \
  }


///
/// Forward declaration for EFI_PCI_EXPRESS_PLATFORM_PROTOCOL.
///
typedef struct _EFI_PCI_EXPRESS_PLATFORM_PROTOCOL  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL;

///
/// Related Definitions for EFI_PCI_EXPRESS_DEVICE_POLICY
///
// Glossary of PCIe terminologies used:-
//  RC = Root Complex
//  RP = Root Port
//  EP = Endpoint
//  RCiEP = Root Complex integrated Endpoint

///
/// EFI encoding used in notification to the platform, for any of the PCI Express
/// capabilities feature state, to indicate that it is not a supported feature,
/// or its present state is unknown
///
#define EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO            0xFF
#define EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE  0xFE

///
/// The EFI_PCI_EXPRESS_DEVICE_POLICY size is fixed as per its definition corresponding
/// to its version, with each byte field represents one PCI Express feature and
/// its bitmask define the legal combinations to represent all the valid combinations
/// of its attributes, defined in the PCI Express Base Specification.
///
typedef struct {

  //
  // For PCI Express feature - Maximum Payload Size
  //
  // Bit 0 - 2 should match with the definition of the "Max_Payload_Size" register
  // field of the Device Control Register (Offset 08h) in the PCI Express Base
  // Specification Revision 5, section 7.5.3.4 , for a valid device policy.
  // The device policy AUTO shall be used to set the payload size as per the Device
  // Capability register "Max_Payload_Size Supported" field, and by aligning to a
  // common value supported among all the devices in the tree.
  // The device policy NOT_APPLICABLE shall be used to skip programming of the
  // maximum payload size of a device.
  //
  UINT8                          MaxPayloadSize;

  //
  // For PCI Express feature - Maximum Read Request Size
  //
  // Bit 0 - 2 should match with the definition of the "Max_Read_Request_Size"
  // register field of the Device Control Register (Offset 08h) in the PCI Express
  // Base Specification Revision 5, section 7.5.3.4, for a valid device policy.
  // The device policy AUTO shall be used to set the Memory read request size as
  // per the device "Max_Payload_Size" value.
  // The device policy NOT_APPLICABLE shall be used to skip programming of the
  // Maximum Read Request Size in the Device Control register
  //
  UINT8                          MaxReadRequestSize;

  //
  // For PCI Express feature - Extended Tag
  //
  // Bit 0 = 1 enables the 8b Extended Tag, else 5b Extended Tag.
  // Bit 1 = 1 enables the 10b Extended Tag as a requester, else Bit 0 value gets
  // applied.
  // Bit 0 should match with the definition of the Device Control register Extended
  // Tag Field Enable (bit 8) of the PCI Express Base Specification Revision 5,
  // section 7.5.3.4.
  // Bit 1 should match with the definition of the Device Control2 register 10-Bit
  // Tag Requester Enable (bit 12) of the PCI Express Base Specification Revision 5,
  // section 7.5.3.16.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  // The device policy from platform can be different for the RP and its EP device;
  // however if the device capability does not match with the policy than it shall
  // be ignored. For example; if the policy for EP is to set the 10b Extended Tag
  // than EP device capability as a requester and its RP completer capability is
  // checked. If platform ask policy change only for RP than its device capability
  // is checked to enable the 10b Extended Tag.
  //
  UINT8                          ExtendedTag;

  //
  // For PCI Express feature - Relaxed Ordering
  //
  // Bit 0 = 1 Enable Relaxed Ordering, else disable.
  // Bit 0 should match with the definition of the Device Control register Enable
  // Relaxed Ordering (bit 4) of the PCI Express Base Specification Revision 5,
  // section 7.5.3.4.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  //
  UINT8                          RelaxedOrdering;

  //
  // For PCI Express feature - No-Snoop
  //
  // Bit 0 = 1 Enable No Snoop, else disable.
  // Bit 0 should match with the definition of the Device Control register Enable
  // No Snoop (bit 11) of the PCI Express Base Specification Revision 5, section
  // 7.5.3.4.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  //
  UINT8                          NoSnoop;

  //
  // For PCI Express feature - ASPM state
  //
  // Bit 0 - 1 should match with the definition of the "ASPM Control" register field
  // of the Link Control Register (Offset 10h) in the PCI Express Base Specification
  // Revision 5, section 7.5.3.7.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  // The device policy of the PCIe Root Port only shall be consider for the entire
  // device tree; ignored for all other devices (EP and RCiEP).
  //
  UINT8                          AspmControl;

  //
  // For PCI Express feature - Common Clock Configuration
  //
  // The device policy AUTO shall be used to set the Common Clock Configuration
  // (bit 6) of the Link Control register as per the device's Link Status register
  // Slot Clock Configuration (bit 12) value.
  // The device policy NOT_APPLICABLE shall be used to skip programming of the Common
  // Clock Configuration of the device.
  //
  UINT8                          CommonClockConfiguration;

  //
  // For PCI Express feature - Atomic Op
  //
  // Bit 0 = 1 to enable the device as the AtomicOp Requester, else disable
  // Bit 1 = 1 to block the incoming AtomicOp Requests going out of its Egress
  // Port (enable Egress blocking for Switch upstream/downstream ports and RP).
  // Bit 0,1 should match with the definition of the of the Device Control2 Register
  // (offset 28h) bit 6,7 named "AtomicOp Requester Enable" and "AtomicOp Egress
  // Blocking" respectively, as in the PCI Express Base Specification Revision
  // 5, section 7.5.3.16.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  // The Bit 1 device policy shall be applied only based on the Device Capability2
  // register field "AtomicOp Routing Supported" (bit 6).
  //
  UINT8                          AtomicOp;

  //
  // For PCI Express feature - LTR
  //
  // Bit 0 = 1 enables the LTR mechanism, else disables.
  // Bit 0 should match with the definition of "LTR Mechanism Enable" (bit 10)
  // of the Device Control 2 Register (Offset 28h) in the PCI Express Base Specification
  // Revision 5, section 7.5.3.16.
  // As per its specification, LTR can be enabled on a EP device if all the devices
  // along the path to RC supports the LTR capability and are enabled.
  // The platform is responsible to provide consist device policy for all the
  // devices in an hierarchy; if RP policy is enable and EP policy is disable,
  // they are permissible but no usage of feature as EP will not send LTR message
  // to RP. Platform should not provide policy for RP as disable and EP as enable,
  // as it shall result in Unsupported Request error. To avoid erroneous scenario
  // even the EP shall be disabled, along with its RP.
  // The device policy AUTO or NOT_APPLICABLE provided to EP shall be treated as
  // mo LTR programming from the path of EP to parent RP. If same is provided for
  // RP than its policy is replaced with the policy of its child device.
  // The device policy is applicable if that is supported by LTR capability.
  //
  UINT8                          Ltr;

  //
  // For PCI Express feature - PTM
  //
  // Bit 0 = PTM Enable, if 1 enables the PTM mechanism as per device role's
  // capability, else disables the PTM mechanism of the device.
  // Bit 0 should match with the definition of the PCI Express Base Specification
  // Revision 5, section 7.9.16.3 of the PTM Control Register (offset 08h).
  // The device policy AUTO shall be used to set the PTM feature as per the device
  // PTM Capability structure.
  // The device policy NOT_APPLICABLE shall be used to prevent the PTM feature
  // enabling for certain device, or for entire PCIe hierarchy.
  // It is platform responsibility to maintain valid policies for all the PCIe
  // devices in the device tree. Invalid combination of device policies in the
  // device tree shall be ignored (for example port is provided wtih NOT_APPLICABLE
  // while the EP device is provided with the AUTO policy).
  //
  UINT8                          Ptm;

  //
  // For PCI Express feature - Completion Timeout
  //
  // Bit 0 - 4 should match with the definition of the register fields "Completion
  // Timeout Value" (bit 0 - 3), and "Completion Timeout Disable" (bit 4) of the
  // PCI Express Base Specification Revision 5, section 7.5.3.16 Device Control
  // 2 Register (Offset 28h), for a valid device policy.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  // The device policy is applicable if that is supported in the device capability.
  //
  UINT8                          CompletionTimeout;

  //
  // For PCI Express feature - Clock Power Management
  //
  // Bit 0 = Enable Clock Power Management (value 1), else disable.
  // Bit 0 should match with the definition of the PCI Express Base Specification
  // Revision 5, section 7.5.3.7 of the Link Control Register (offset 10h) bit 8.
  // This device policy shall be applied based on device Link Capability register
  // value of bit 18 of offset 0Ch.
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  //
  UINT8                          ClockPowerManagement;

  //
  // For PCI Express feature - L1 PM Substates
  //
  // Bit 0 = PCI-PM L1.2 Enable, Bit 1 = PCI-PM L1.1 Enable, Bit 2 = ASPM L1.2 Enable,
  // Bit 3 = ASPM L1.1 Enable; if set enables L1 substates else disables.
  // Bit 0 - 3 should match with the definition of the PCI Express Base Specification
  // Revision 5, section 7.8.3.3 of L1 PM Substates Control 1 Register (Offset 08h).
  // The device policy AUTO or NOT_APPLICABLE shall be treated as "do not touch".
  // The device policy of only the PCIe Root Port should be consider for the entire
  // device tree; ignored for all other devices (EP and RCiEP).
  //
  UINT8                           L1PmSubstates;
} EFI_PCI_EXPRESS_DEVICE_POLICY;

///
/// The EFI_PCI_EXPRESS_DEVICE_STATE is an alias of EFI_PCI_EXPRESS_DEVICE_POLICY,
/// used for notification to the platform about the device PCI Express features
/// state, through the NotifyDeviceState interface method.
/// For any of the device's PCIe features that are not supported, or its
/// state is unknown, it will be returned as EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE.
///
typedef EFI_PCI_EXPRESS_DEVICE_POLICY EFI_PCI_EXPRESS_DEVICE_STATE;

/**
  Interface to receive the PCIe features device policy from platform.

  The consumer driver(s), like PCI Bus driver and PCI Host Bridge Resource Allocation
  Protocol drivers; can call this member function to retrieve the platform policies
  specific to PCI device, related to its PCI Express capabilities. The producer of
  this protocol shall be the platform itself to provide the device-specific policies.

  The GetDevicePolicy() member function is meant to return data about the PCIe
  features like the MPS, MRRS, Extended Tag, ASPM, etc. The details about
  these PCIe features can be obtained from the PCI Express Base Specification (Rev.4
  or 5). The EFI encodings are defined in the EFI_PCI_EXPRESS_PLATFORM_POLICY,
  see the Related Definition section for this. This member function
  will use the associated EFI handle of the root bridge instance and the PCI address
  of type EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS to determine the physical
  PCI device within the chipset.
  It is caller's responsibility to allocate the buffer and pass its pointer of
  type EFI_PCI_EXPRESS_DEVICE_POLICY to this member function, to get its device
  -specific policy data.
  The caller is required to initialize the input buffer with either of two values:-
  1.  EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE: for those PCI Express features
      which are not supported by the calling driver
  2.  EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO: for those PCI Express features which
      are supported by the calling driver

  @param[in]      This          Pointer to the  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL instance.
  @param[in]      RootBridge    EFI handle of associated root bridge to the PCI device
  @param[in]      PciAddress    The address of the PCI device on the PCI bus.
                                This address can be passed to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
                                member functions to access the PCI configuration space of the
                                device. See UEFI 2.1 Specification for the definition
                                of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS.
  @param[in]      Size          The size, in bytes, of the input buffer, in next parameter.
  @param[in,out]  PciePolicy    The pointer to buffer containing the device policies for
                                PCIe features like, the MPS, MRRS, etc. Type
                                EFI_PCI_EXPRESS_DEVICE_POLICY is defined above.


  @retval EFI_SUCCESS            The function completed successfully, may returns
                                 platform policy data for the given PCI component
  @retval EFI_UNSUPPORTED        PCI component belongs to PCI topology but not
                                 part of chipset to provide the platform policy
  @retval EFI_INVALID_PARAMETER  If any of the input parameters are passed with
                                 invalid data

 **/
typedef
EFI_STATUS
(EFIAPI * EFI_PCI_EXPRESS_GET_DEVICE_POLICY) (
  IN CONST  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL             *This,
  IN        EFI_HANDLE                                    RootBridge,
  IN        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS   PciAddress,
  IN        UINTN                                         Size,
  IN OUT    EFI_PCI_EXPRESS_DEVICE_POLICY                 *PciePolicy
);

/**
  Notifies the platform about the PCI device current state of its PCIe capabilites.

  The PCI Bus driver or PCI Host Bridge Resource Allocation Protocol drivers
  can call this member function to notify the present PCIe configuration state
  of the PCI device, to the platform. This is expected to be invoked after the
  completion of the PCI enumeration phase.

  The NotifyDeviceState() member function is meant to provide configured
  data, to the platform about the PCIe features which would be supported by the
  PCI Bus driver in future; like for example the MPS, MRRS, Extended Tag, ASPM,
  etc. The details about this PCI features can be obtained from the PCI Express
  Base Specification.

  The EFI encodings and data types used to report out the current state, are
  same as those that were used by the platform to return the device-specific
  platform policies, in the EFI_PCI_EXPRESS_DEVICE_POLICY (see the "Related
  Definition" section for this). The difference is that it should return the actual
  state in the EFI_PCI_EXPRESS_DEVICE_STATE; without any macros corresponding
  to EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO. Note that, if the notifying driver does not
  support any PCIe feature than it shall return its corresponding value as
  EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE.

  This member function will use the associated EFI handle of the PCI IO Protocol
  to address the physical PCI device within the chipset. It is caller's
  responsibility to allocate the buffer and pass its pointer to this member
  function.

  @param[in]  This          Pointer to the  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL instance.
  @param[in]  PciDevice     The associated PCI IO Protocol handle of the PCI
                            device. Type EFI_HANDLE is defined in
                            InstallProtocolInterface() in the UEFI 2.1 Specification
  @param[in] Size           The size, in bytes, of the input buffer in next parameter.
  @param[in] PcieState      The pointer to device configuration state with respect
                            to other PCI features like, the MPS, MRRS, etc. Type
                            EFI_PCI_EXPRESS_DEVICE_STATE is an alias to
                            EFI_PCI_EXPRESS_DEVICE_POLICY, as defined above.


  @retval EFI_SUCCESS             This function completed successfully, the platform
                                  was able to identify the PCI device successfully
  @retval EFI_INVALID_PARAMETER   Platform was not able to identify the PCI device;
                                  or its PCI Express device configuration state
                                  provided has invalid data.

 **/
typedef
EFI_STATUS
(EFIAPI * EFI_PCI_EXPRESS_NOTIFY_DEVICE_STATE) (
  IN CONST  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL     *This,
  IN        EFI_HANDLE                            PciDevice,
  IN        UINTN                                 Size,
  IN        EFI_PCI_EXPRESS_DEVICE_STATE          *PcieState
  );

///
/// The EFI_PCI_EXPRESS_PLATFORM_POLICY is use to exchange feature policy, at a
/// system level. Each boolean member represents each PCI Express feature defined
/// in the PCI Express Base Specification. The value TRUE on exchange indicates
/// support for this feature configuration, and FALSE indicates not supported,
/// or no configuration required on return data. The order of members represent
/// PCI Express features that are fixed for this protocol definition, and should
/// be aligned with the definition of the EFI_PCI_EXPRESS_DEVICE_POLICY.
///
typedef struct {
  //
  // For PCI Express feature - Maximum Payload Size
  //
  BOOLEAN  MaxPayloadSize;
  //
  // For PCI Express feature - Maximum Read Request Size
  //
  BOOLEAN  MaxReadRequestSize;
  //
  // For PCI Express feature - Extended Tag
  //
  BOOLEAN  ExtendedTag;
  //
  // For PCI Express feature - Relaxed Ordering
  //
  BOOLEAN  RelaxedOrdering;
  //
  // For PCI Express feature - No-Snoop
  //
  BOOLEAN  NoSnoop;
  //
  // For PCI Express feature - ASPM state
  //
  BOOLEAN  Aspm;
  //
  // For PCI Express feature - Common Clock Configuration
  //
  BOOLEAN  CommonClockConfiguration;
  //
  // For PCI Express feature - Atomic Op
  //
  BOOLEAN  AtomicOp;
  //
  // For PCI Express feature - LTR
  //
  BOOLEAN  Ltr;
  //
  // For PCI Express feature - PTM
  //
  BOOLEAN  Ptm;
  //
  // For PCI Express feature - Completion Timeout
  //
  BOOLEAN  CompletionTimeout;
  //
  // For PCI Express feature - Clock Power Management
  //
  BOOLEAN  ClockPowerManagement;
  //
  // For PCI Express feature - L1 PM Substates
  //
  BOOLEAN  L1PmSubstates;

} EFI_PCI_EXPRESS_PLATFORM_POLICY;

/**
  Informs the platform about its PCI Express features support capability and it
  accepts request from platform to initialize those features.

  The caller shall first invoke this interface to inform platform about the list
  of PCI Express feature that are supported by passing its value as TRUE, and value
  FALSE for unsupported features, in the buffer of type EFI_PCI_EXPRESS_PLATFORM_POLICY.
  On return, it expects the list of supported PCI Express features as a requirement
  from platform; if its return value is TRUE. The caller shall ignore invalid TRUE
  values.

  The caller shall treat this list of PCI Express features as the global platform
  requirement; and corresponding to that feature list it expects device-specific
  policy through the interface GetDevicePolicy() to configure individual PCI devices.
  For example, say that the caller indicates first 8 PCI Express features in the
  list of EFI_PCI_EXPRESS_PLATFORM_POLICY, and platform wants only 5 PCI Express
  features from that list to be configured by the caller. Thus, based on feature
  list recorded here, the device-specific policy returned in the GetDevicePolicy()
  for every PCI device in the system; and the caller shall configure only 5 PCI
  Express features accordingly.

  The protocol producing driver shall use the size input parameter to determine
  the length of the buffer of type EFI_PCI_EXPRESS_PLATFORM_POLICY, and to also
  determine version of the caller. If the size of the input buffer is more than
  what its supporting version (indicated through the Revision data member of the
  protocol) than it shall return EFI_INVALID_PARAMETER.

  The error code in returned status essential means that the caller cannot initialize
  any PCI Express features. Thus, this interface would be primary interface for the
  caller to initialize the PCI Express features for the platform, apart from
  obtaining the handle of this protocol.

  @param[in]      This             Pointer to the  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL
                                   instance.
  @param[in]      Size             type UINTN to indicate the size of the input
                                   buffer
  @param[in,out]  PlatformPolicy   Pointer to the EFI_PCI_EXPRESS_PLATFORM_POLICY.
                                   On input, the caller indicates the list of supported
                                   PCI Express features. On output, the platform
                                   indicates the required features out of the list
                                   that needs to be initialized by the caller.

  @retval EFI_SUCCESS              The function completed successfully, returns
                                   platform policy for the PCI Express features
  @retval EFI_INVALID_PARAMETER    If any of the input parameters are passed with
                                   invalid data; like the pointer to the protocol
                                   is not valid, the size of the input buffer is
                                   greater than the version supported, the pointer
                                   to buffer is NULL.

 **/
typedef
EFI_STATUS
(EFIAPI * EFI_PCI_EXPRESS_GET_POLICY) (
  IN CONST  EFI_PCI_EXPRESS_PLATFORM_PROTOCOL     *This,
  IN        UINTN                                 Size,
  IN OUT    EFI_PCI_EXPRESS_PLATFORM_POLICY       *PlatformPolicy
);

///
/// The revision of the PCI Express Protocol is represented as 32-bit value
/// with the lower WORD representing the Minor value and upper WORD
/// representing the Major value.
/// As per the specification of this protocol, the revision is 1.1.
/// Any driver utilizing this protocol shall use the revision number to
/// maintain the backward compatibility.
///
#define EFI_PCI_EXPRESS_PLATFORM_PROTOCOL_REVISION 0x00010001

///
/// This protocol provides the interface between the PCI bus driver/PCI Host
/// Bridge Resource Allocation driver and a platform-specific driver to describe
/// the unique PCI Express features of a platform.
///
struct _EFI_PCI_EXPRESS_PLATFORM_PROTOCOL {
  ///
  /// The revision of this PCIe Platform Protocol
  ///
  UINT32                                 Revision;
  ///
  /// Retrieves the PCIe device-specific platform policy regarding enumeration.
  ///
  EFI_PCI_EXPRESS_GET_DEVICE_POLICY      GetDevicePolicy;
  ///
  /// Informs the platform about the PCIe capabilities programmed, based on the
  /// present state of the PCI device
  ///
  EFI_PCI_EXPRESS_NOTIFY_DEVICE_STATE    NotifyDeviceState;
  ///
  /// Retrieve the platform policy to select the PCI Express features
  ///
  EFI_PCI_EXPRESS_GET_POLICY             GetPolicy;

};

extern EFI_GUID   gEfiPciExpressPlatformProtocolGuid;

#endif
