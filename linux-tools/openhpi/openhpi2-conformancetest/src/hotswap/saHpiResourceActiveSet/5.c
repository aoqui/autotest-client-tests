/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or 
 *   (at your option) any later version.
 *   This program is distributed in the hope that it will be useful, 
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *   GNU General Public License for more details. 
 *   You should have received a copy of the GNU General Public License 
 *   along with this program; if not, write to the Free Software 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *   USA 
 *
 * Author(s):
 *      Carl McAdams <carlmc@us.ibm.com>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiResourceActiveSet
 * Description:
 *   Call on a resource which is not in the Insertion or Extraction Pending state.
 *   saHpiResourceActiveSet() returns SA_ERR_HPI_INVALID_REQUEST.
 * Line:        P139-16:P139-18
 *    
 */
#include <stdio.h>
#include "saf_test.h"

int TestCase(SaHpiSessionIdT session,
	     SaHpiResourceIdT resource, int *returnvalue, SaHpiBoolT * restore)
{

	SaErrorT status;
	if (*returnvalue == SAF_TEST_UNKNOWN) {
		status = saHpiResourceActiveSet(session, resource);
		if (status != SA_ERR_HPI_INVALID_REQUEST) {
			e_print(saHpiResourceActiveSet,
				SA_ERR_HPI_INVALID_REQUEST, status);
			*returnvalue = SAF_TEST_FAIL;

		} else {
			*restore = SAHPI_TRUE;
			*returnvalue = SAF_TEST_PASS;
		}
	}
	return (*returnvalue);
}
int Activate(SaHpiSessionIdT session,
	     SaHpiResourceIdT resource, int *returnvalue, SaHpiBoolT * restore)
{

	SaErrorT status;
	if ((*returnvalue == SAF_TEST_UNKNOWN) || (*restore != SAHPI_FALSE)) {
		status = saHpiResourceActiveSet(session, resource);
		if (*restore == SAHPI_FALSE) {
			if (status != SA_OK) {
				e_print(saHpiResourceActiveSet, SA_OK, status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}
	}
	return (*returnvalue);
}
int Inactivate(SaHpiSessionIdT session,
	       SaHpiResourceIdT resource,
	       int *returnvalue, SaHpiBoolT * restore)
{

	SaErrorT status;
	if ((*returnvalue == SAF_TEST_UNKNOWN) || (*restore != SAHPI_FALSE)) {
		status = saHpiResourceInactiveSet(session, resource);
		if (*restore == SAHPI_FALSE) {
			if (status != SA_OK) {
				e_print(saHpiResourceInactiveSet,
					SA_OK, status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}
	}
	return (*returnvalue);
}
int Insert(SaHpiSessionIdT session,
	   SaHpiResourceIdT resource, int *returnvalue, SaHpiBoolT * restore)
{

	SaErrorT status;

	if ((*returnvalue == SAF_TEST_UNKNOWN) || (*restore != SAHPI_FALSE)) {
		status = saHpiHotSwapActionRequest(session,
						   resource,
						   SAHPI_HS_ACTION_INSERTION);
		if (*restore == SAHPI_FALSE) {
			if (status != SA_OK) {
				e_print(saHpiHotSwapActionRequest,
					SA_OK, status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}
		// After placed in the Insertion Pending mode, cancel the policy
		// We are only testing from the extraction pending state.
		status = saHpiHotSwapPolicyCancel(session, resource);
		if (*restore == SAHPI_FALSE) {
			if (!((status == SA_OK) ||
			      (status == SA_ERR_HPI_INVALID_REQUEST))) {
				e_print(saHpiHotSwapPolicyCancel,
					SA_OK || SA_ERR_HPI_INVALID_REQUEST,
					status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}

	}
	return (*returnvalue);
}
int Extract(SaHpiSessionIdT session,
	    SaHpiResourceIdT resource, int *returnvalue, SaHpiBoolT * restore)
{

	SaErrorT status;
	if ((*returnvalue == SAF_TEST_UNKNOWN) || (*restore != SAHPI_FALSE)) {
		status = saHpiHotSwapActionRequest(session,
						   resource,
						   SAHPI_HS_ACTION_EXTRACTION);
		if (*restore == SAHPI_FALSE) {
			if (status != SA_OK) {
				e_print(saHpiHotSwapActionRequest,
					SA_OK, status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}
		status = saHpiHotSwapPolicyCancel(session, resource);
		if (*restore == SAHPI_FALSE) {
			if (!((status == SA_OK) ||
			      (status == SA_ERR_HPI_INVALID_REQUEST))) {
				e_print(saHpiHotSwapPolicyCancel,
					SA_OK || SA_ERR_HPI_INVALID_REQUEST,
					status);
				*returnvalue = SAF_TEST_UNRESOLVED;
			}
		}
	}
	return (*returnvalue);
}

int Test_Resource(SaHpiSessionIdT session,
		  SaHpiRptEntryT report, callback2_t func)
{
	SaErrorT status;
	int retval = SAF_TEST_UNKNOWN;
	SaHpiHsStateT State;
	SaHpiBoolT Restore = SAHPI_FALSE;

	if ((report.ResourceCapabilities &
	     SAHPI_CAPABILITY_MANAGED_HOTSWAP) &&
	    (report.ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
		status =
		    saHpiHotSwapStateGet(session, report.ResourceId, &State);
		if (status != SA_OK) {
			e_print(saHpiHotSwapStateGet, SA_OK, status);
			retval = SAF_TEST_UNRESOLVED;
		}
		if (retval == SAF_TEST_UNKNOWN) {
			switch (State) {
			case SAHPI_HS_STATE_ACTIVE:
				TestCase(session, report.ResourceId, &retval,
					 &Restore);
				break;
			case SAHPI_HS_STATE_EXTRACTION_PENDING:
				Activate(session, report.ResourceId, &retval,
					 &Restore);
				TestCase(session, report.ResourceId, &retval,
					 &Restore);
				Extract(session, report.ResourceId, &retval,
					&Restore);
				break;
			case SAHPI_HS_STATE_INSERTION_PENDING:
				Inactivate(session, report.ResourceId, &retval,
					   &Restore);
				TestCase(session, report.ResourceId, &retval,
					 &Restore);
				Insert(session, report.ResourceId, &retval,
				       &Restore);
				break;
			case SAHPI_HS_STATE_INACTIVE:
				TestCase(session, report.ResourceId, &retval,
					 &Restore);
				break;
			default:
				printf
				    ("  Function \"saHpiHotSwapStateGet\" works abnormally!\n");
				printf("  state recieved is not valid!\n");
				retval = SAF_TEST_UNRESOLVED;
			}
		}
	} else {
		// Not a Hot Swap supported Resource
		retval = SAF_TEST_NOTSUPPORT;
	}
	return (retval);
}

int main()
{
	int retval = SAF_TEST_UNKNOWN;

	retval = process_all_domains(Test_Resource, NULL, NULL);

	return retval;
}
