/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

    Etripator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Etripator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "labels.h"
#include "cfg.h"
#include "message.h"

#define LABEL_ARRAY_INC 16

/* Initialize label repository */
int initializeLabelRepository(LabelRepository* iRepository)
{
	iRepository->size = LABEL_ARRAY_INC;
	iRepository->labels = (Label*)malloc(iRepository->size * sizeof(Label));
	if(iRepository->labels == NULL)
	{
		return 0;
	}
	
	iRepository->last = 0;
	return 1;
}

/* Reset label repsitory */
void resetLabelRepository(LabelRepository* iRepository)
{
	iRepository->last = 0;
}

/* Delete label repository */
void deleteLabelRepository(LabelRepository* iRepository)
{
	if(iRepository->labels != NULL)
	{
		free(iRepository->labels);
		iRepository->labels = NULL;
	}
	iRepository->size = iRepository->last = 0;
}


/* Push label to repository */
int pushLabel(LabelRepository* iRepository, uint16_t iOffset)
{
	if(iRepository->last >= iRepository->size)
	{
		Label *ptr;
		/* Expand array */
		iRepository->size += LABEL_ARRAY_INC;
		ptr = (Label*)realloc(iRepository->labels, iRepository->size * sizeof(Label));
		if(ptr == NULL)
		{
			deleteLabelRepository(iRepository);
			return 0;
		}
		
		iRepository->labels = ptr;
	}
	
	/* Push offset */
	iRepository->labels[iRepository->last].offset	    = iOffset;
	iRepository->labels[iRepository->last].displacement = 0;
		
	++iRepository->last;
	return 1;
}

/* Compare two labels */
static int compareLabels(const void* iL0, const void* iL1)
{
	const Label *l0 = (const Label*)iL0;
	const Label *l1 = (const Label*)iL1;
	
	return (l0->offset - l1->offset);
}

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository* iRepository)
{
	if((iRepository->labels == NULL) || (iRepository->last < 2))
		return;

	/* Sort offsets */
	qsort(iRepository->labels, iRepository->last, sizeof(Label), compareLabels);
}


/* Label CFG section start */
static int labelBeginSectionCallback(void *data, const char *sectionName)
{
    (void)data;
    (void)sectionName;
    return 1;
}
/* Label CFG section end */
static int labelEndSectionCallback(void *data)
{
    (void)data;
    return 1;
}
/* Add name/label to repository */
int labelKeyValueCallback(void *data, const char* key, const char* value)
{
    LabelRepository *repository = (LabelRepository*)data;
    (void)key;
    if(NULL == repository)
    {
        return 0;
    }
    else
    {
        uint16_t addr = (uint16_t)(strtoul(value, NULL, 16) & 0xffff);
        return pushLabel(repository, addr);
    }
}

/**
 * Load labels from a cfg file.
 * \param [in]  filename Cfg file.
 * \param [out] reposity Label repository.
 * \return 1 if the labels contained in the file was succesfully added to the repository.
 *         0 if an error occured.
 */
int loadLabels(const char* filename, LabelRepository* repository)
{
    CFG_ERR err;
    struct CFGPayload payload;

    payload.data = repository;
    payload.beginSectionCallback = labelBeginSectionCallback;
    payload.endSectionCallback   = labelEndSectionCallback;
    payload.keyValueCallback     = labelKeyValueCallback;

    err = ParseCFG(filename, &payload);
    if(CFG_OK != err)
    {
        ERROR_MSG("Failed to load labels from %s : %s", filename, GetCFGErrorMsg(err));
        return 0;
    }
    return 1;
}
