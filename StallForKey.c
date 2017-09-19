/*++

Copyright (c) 2005, Intel Corporation
Copyright (c) 2017, Wei-Lun Chao <bluebat@member.fsf.org> (UEFI porting)
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the Eclipse Public License
which accompanies this distribution.  The full text of the license may be found at
http://www.opensource.org/licenses/eclipse-1.0.php

THE PROGRAM IS DISTRIBUTED UNDER THE ECLIPSE PUBLIC LICENSE (EPL) ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  StallForKey.c

Abstract:

  Waiting for a key input.

--*/

//
// Includes
//

#include <efi.h>
#include <efilib.h>

//
// Global definitions
//

#define DEFAULT_WAIT_TIME         10        // 10 seconds

//
// Entry point
//

EFI_STATUS
efi_main (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS      Status;
  UINTN           Argc;
  CHAR16          **Argv;
  UINTN           Seconds;
  EFI_EVENT       TimerEvent;
  EFI_EVENT       WaitList[2];
  UINTN           WaitIndex;
  EFI_INPUT_KEY   Key;

  //
  // Initialize libraries
  //
  InitializeLib(ImageHandle, SystemTable);

  //
  // Process the arguments
  //
  Argc = GetShellArgcArgv(ImageHandle, &Argv);
  Seconds = DEFAULT_WAIT_TIME;
  if (Argc > 1) {
    Seconds = Atoi (Argv[1]);
  }

  //
  // Create timer event
  //
  Status = BS->CreateEvent (
                 EVT_TIMER,
                 0,
                 NULL,
                 NULL,
                 &TimerEvent
                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Create event. %a:%d:%r\n", __FILE__, __LINE__, Status));
    return Status;
  }

  //
  // Set up the timer
  //
  Status = BS->SetTimer (
                 TimerEvent,
                 TimerPeriodic,
                 10000000                   // 1 second
                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Set timer. %a:%d:%r\n", __FILE__, __LINE__, Status));
    BS->CloseEvent (TimerEvent);
    return Status;
  }

  //
  // Set up waiting list
  //
  WaitList[0] = ST->ConIn->WaitForKey;
  WaitList[1] = TimerEvent;

  //
  // Do it until timeout
  //
  while (Seconds != 0) {
    //
    // Print the prompt information (The last blank char is used to clean up
    // the last string. Don't remove it.)
    //
    Print (L"Press any key within %d seconds \r", Seconds);

    //
    // Wait for timeout or key input
    //
    Status = BS->WaitForEvent (
                   2,
                   WaitList,
                   &WaitIndex
                   );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Wait for event. %a:%d:%r\n", __FILE__, __LINE__, Status));
      break;
    }

    //
    // Check the triggered event
    //
    if (WaitIndex == 0) {
      //
      // Key input event is triggered
      //
      Key.ScanCode = SCAN_NULL;
      Status = ST->ConIn->ReadKeyStroke (ST->ConIn, &Key);
      if (!EFI_ERROR (Status)) {
        break;
      }

      DEBUG ((EFI_D_ERROR, "Read key stroke. %a:%d:%r\n", __FILE__, __LINE__, Status));
    } else {
      //
      // Timer event is triggered
      //
      Seconds --;
    }
  }

  //
  // Close the timer event
  //
  BS->CloseEvent (TimerEvent);

  //
  // Done
  //
  if (Seconds == 0) {
    return EFI_TIMEOUT;
  } else {
    return EFI_SUCCESS;
  }
}
