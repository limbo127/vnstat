#include "common.h"
#include "dbsql.h"
#include "misc.h"
#include "image.h"
#include "vnstati.h"

void initimagecontent(IMAGECONTENT *ic)
{
	ic->showheader = 1;
	ic->showedge = 1;
	ic->showlegend = 1;
	ic->altdate = 0;
	ic->headertext[0] = '\0';
}

void drawimage(IMAGECONTENT *ic)
{
	switch (cfg.qmode) {
		case 1:
			drawlist(ic, "day");
			break;
		case 2:
			drawlist(ic, "month");
			break;
		case 3:
			drawlist(ic, "top");
			break;
		case 4:
			drawlist(ic, "year");
			break;
/*		case 5:
			if (cfg.slayout) {
				drawsummary(ic, 0, 0);
			} else {
				drawoldsummary(ic, 0, 0);
			}
			break;
		case 51:
			if (cfg.slayout) {
				drawsummary(ic, 1, cfg.hourlyrate);
			} else {
				drawoldsummary(ic, 1, cfg.hourlyrate);
			}
			break;
		case 52:
			if (cfg.slayout) {
				drawsummary(ic, 2, cfg.hourlyrate);
			} else {
				drawoldsummary(ic, 2, cfg.hourlyrate);
			}
			break;*/
		case 7:
			drawhourly(ic, cfg.hourlyrate);
			break;
		default:
			printf("Error: No such query mode: %d\n", cfg.qmode);
			exit(EXIT_FAILURE);
			break;
	}

	/* enable background transparency if needed */
	if (cfg.transbg) {
		gdImageColorTransparent(ic->im, ic->cbackground);
	}
}

void colorinit(IMAGECONTENT *ic)
{
	int rgb[3];

	/* text, edge and header colors */
	hextorgb(cfg.ctext, rgb);
	ic->ctext = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("ctext", ic->ctext, cfg.ctext, rgb);
	hextorgb(cfg.cedge, rgb);
	ic->cedge = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cedge", ic->cedge, cfg.cedge, rgb);
	hextorgb(cfg.cheader, rgb);
	ic->cheader = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cheader", ic->cheader, cfg.cheader, rgb);
	hextorgb(cfg.cheadertitle, rgb);
	ic->cheadertitle = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cheadertitle", ic->cheadertitle, cfg.cheadertitle, rgb);
	hextorgb(cfg.cheaderdate, rgb);
	ic->cheaderdate = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cheaderdate", ic->cheaderdate, cfg.cheaderdate, rgb);

	/* lines */
	hextorgb(cfg.cline, rgb);
	ic->cline = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cline", ic->cline, cfg.cline, rgb);
	if (cfg.clinel[0] == '-') {
		modcolor(rgb, 50, 1);
	} else {
		hextorgb(cfg.clinel, rgb);
	}
	ic->clinel = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("clinel", ic->clinel, cfg.clinel, rgb);

	/* background */
	hextorgb(cfg.cbg, rgb);
	ic->cbackground = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cbackground", ic->cbackground, cfg.cbg, rgb);
	modcolor(rgb, -35, 0);
	ic->cvnstat = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cvnstat", ic->cvnstat, cfg.cbg, rgb);
	hextorgb(cfg.cbg, rgb);
	modcolor(rgb, -15, 0);
	ic->cbgoffset = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("cbgoffset", ic->cbgoffset, cfg.cbg, rgb);

	/* rx */
	hextorgb(cfg.crx, rgb);
	ic->crx = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("crx", ic->crx, cfg.crx, rgb);
	if (cfg.crxd[0] == '-') {
		modcolor(rgb, -50, 1);
	} else {
		hextorgb(cfg.crxd, rgb);
	}
	ic->crxd = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("crxd", ic->crxd, cfg.crxd, rgb);

	/* tx */
	hextorgb(cfg.ctx, rgb);
	ic->ctx = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("ctx", ic->ctx, cfg.ctx, rgb);
	if (cfg.ctxd[0] == '-') {
		modcolor(rgb, -50, 1);
	} else {
		hextorgb(cfg.ctxd, rgb);
	}
	ic->ctxd = gdImageColorAllocate(ic->im, rgb[0], rgb[1], rgb[2]);
	colorinitcheck("ctxd", ic->ctxd, cfg.ctxd, rgb);
}

void colorinitcheck(const char *color, const int value, const char *cfgtext, const int *rgb)
{
	if (value==-1) {
		printf("Error: ImageColorAllocate failed.\n");
		printf("       C: \"%s\" T: \"%s\" RGB: %d/%d/%d\n", color, cfgtext, rgb[0], rgb[1], rgb[2]);
		exit(EXIT_FAILURE);
	}
}

void layoutinit(IMAGECONTENT *ic, const char *title, const int width, const int height)
{
	struct tm *d;
	char datestring[64];

	/* get time in given format */
	d = localtime(&ic->interface.updated);
	strftime(datestring, 64, cfg.hformat, d);

	/* background, edges */
	gdImageFill(ic->im, 0, 0, ic->cbackground);
	if (ic->showedge) {
		gdImageRectangle(ic->im, 0, 0, width-1, height-1, ic->cedge);
	}

	/* titlebox with title */
	if (ic->showheader) {
		gdImageFilledRectangle(ic->im, 2+ic->showedge, 2+ic->showedge, width-3-ic->showedge, 24, ic->cheader);
		gdImageString(ic->im, gdFontGetGiant(), 12, 5+ic->showedge, (unsigned char*)title, ic->cheadertitle);
	}

	/* date */
	if (!ic->showheader || ic->altdate) {
		gdImageString(ic->im, gdFontGetTiny(), 5+ic->showedge, height-12-ic->showedge, (unsigned char*)datestring, ic->cvnstat);
	} else {
		gdImageString(ic->im, gdFontGetTiny(), width-(strlen(datestring)*gdFontGetTiny()->w+12), 9+ic->showedge, (unsigned char*)datestring, ic->cheaderdate);
	}

	/* generator */
	gdImageString(ic->im, gdFontGetTiny(), width-114-ic->showedge, height-12-ic->showedge, (unsigned char*)"vnStat / Teemu Toivola", ic->cvnstat);
}

void drawlegend(IMAGECONTENT *ic, const int x, const int y)
{
	if (!ic->showlegend) {
		return;
	}

	/* color legend */
	gdImageString(ic->im, gdFontGetSmall(), x, y, (unsigned char*)"rx     tx", ic->ctext);
	gdImageFilledRectangle(ic->im, x-12, y+4, x-6, y+10, ic->crx);
	gdImageRectangle(ic->im, x-12, y+4, x-6, y+10, ic->ctext);
	gdImageFilledRectangle(ic->im, x+30, y+4, x+36, y+10, ic->ctx);
	gdImageRectangle(ic->im, x+30, y+4, x+36, y+10, ic->ctext);
}

void drawbar(IMAGECONTENT *ic, const int x, const int y, const int len, const uint64_t rx, const uint64_t tx, const uint64_t max)
{
	int l, width = len;

	if ((rx+tx)!=max) {
		width=((rx+tx)/(float)max)*len;
	}

	if (width!=0) {

		if (tx>rx) {
			l=rintf((rx/(float)(rx+tx)*width));

			gdImageFilledRectangle(ic->im, x, y+YBEGINOFFSET, x+l, y+YENDOFFSET, ic->crx);
			gdImageRectangle(ic->im, x, y+YBEGINOFFSET, x+l, y+YENDOFFSET, ic->crxd);

			gdImageFilledRectangle(ic->im, x+l, y+YBEGINOFFSET, x+width, y+YENDOFFSET, ic->ctx);
			gdImageRectangle(ic->im, x+l, y+YBEGINOFFSET, x+width, y+YENDOFFSET, ic->ctxd);

		} else {
			l=rintf((tx/(float)(rx+tx)*width));

			gdImageFilledRectangle(ic->im, x, y+YBEGINOFFSET, x+(width-l), y+YENDOFFSET, ic->crx);
			gdImageRectangle(ic->im, x, y+YBEGINOFFSET, x+(width-l), y+YENDOFFSET, ic->crxd);

			gdImageFilledRectangle(ic->im, x+(width-l), y+YBEGINOFFSET, x+width, y+YENDOFFSET, ic->ctx);
			gdImageRectangle(ic->im, x+(width-l), y+YBEGINOFFSET, x+width, y+YENDOFFSET, ic->ctxd);
		}
	}
}

void drawpole(IMAGECONTENT *ic, const int x, const int y, const int len, const uint64_t rx, const uint64_t tx, const uint64_t max)
{
	int l;

	l = (rx/(float)max)*len;
	gdImageFilledRectangle(ic->im, x, y+(len-l), x+7, y+len, ic->crx);

	l = (tx/(float)max)*len;
	gdImageFilledRectangle(ic->im, x+5, y+(len-l), x+12, y+len, ic->ctx);
}

void drawdonut(IMAGECONTENT *ic, const int x, const int y, const float rxp, const float txp)
{
	int rxarc = 0, txarc = 0;

	if ( (int)(rxp + txp) > 0 ) {
		rxarc = 360 * (rxp / (float)100);
		if ( (int)(rxp + txp) == 100 ) {
			txarc = 360 - rxarc;
		} else {
			txarc = 360 * (txp / (float)100);
		}

		/* fix possible graphical glitch */
		if (!rxarc) {
			rxarc = 1;
		}

		if (!txarc) {
			txarc = 1;
		}
	}

	gdImageFilledArc(ic->im, x, y, DOUTRAD, DOUTRAD, 0, 360, ic->cbgoffset, 0);

	if ( (int)(rxp + txp) > 0 ) {
		gdImageFilledArc(ic->im, x, y, DOUTRAD, DOUTRAD, 270, 270+txarc, ic->ctx, 0);
		gdImageFilledArc(ic->im, x, y, DOUTRAD, DOUTRAD, 270, 270+txarc, ic->ctxd, gdEdged|gdNoFill);
		gdImageFilledArc(ic->im, x, y, DOUTRAD, DOUTRAD, 270+txarc, 270+txarc+rxarc, ic->crx, 0);
		gdImageFilledArc(ic->im, x, y, DOUTRAD, DOUTRAD, 270+txarc, 270+txarc+rxarc, ic->crxd, gdEdged|gdNoFill);

		gdImageFilledArc(ic->im, x, y, DINRAD, DINRAD, 270, 270+txarc, ic->ctxd, gdEdged|gdNoFill);
		gdImageFilledArc(ic->im, x, y, DINRAD, DINRAD, 270+txarc, 270+txarc+rxarc, ic->crxd, gdEdged|gdNoFill);
	}

	gdImageFilledArc(ic->im, x, y, DINRAD-2, DINRAD-2, 0, 360, ic->cbackground, 0);
}

void drawhours(IMAGECONTENT *ic, const int x, const int y, const int rate)
{
	int i, tmax=0, s=0, step, prev=0, diff=0, chour;
	float ratediv;
	uint64_t max=1, scaleunit=0;
	char buffer[32];
	struct tm *d;
	dbdatalist *datalist = NULL, *datalist_i = NULL;
	dbdatalistinfo datainfo;
	HOURDATA hourdata[24];

	for (i=0; i<24; i++) {
		hourdata[i].rx = hourdata[i].tx = 0;
		hourdata[i].date = 0;
	}

	if (!db_getdata(&datalist, &datainfo, ic->interface.name, "hour", 24)) {
		printf("Error: Failed to fetch hour data.\n");
		return;
	}

	if (datainfo.count == 0) {
		return;
	}

	datalist_i = datalist;

	while (datalist_i != NULL) {
		d = localtime(&datalist_i->timestamp);
		if (hourdata[d->tm_hour].date != 0 || ic->interface.updated-datalist_i->timestamp > 86400) {
			datalist_i = datalist_i->next;
			continue;
		}
		hourdata[d->tm_hour].rx = datalist_i->rx;
		hourdata[d->tm_hour].tx = datalist_i->tx;
		hourdata[d->tm_hour].date = datalist_i->timestamp;
		datalist_i = datalist_i->next;
	}
	dbdatalistfree(&datalist);

	ic->current = ic->interface.updated;
	chour = localtime(&ic->current)->tm_hour;

	if (cfg.rateunit) {
		ratediv = 450;      /* x * 8 / 3600 */
	} else {
		ratediv = 3600;
	}

	/* tmax (time max) = current hour */
	/* max = transfer max */

	for (i = 0; i < 24; i++) {
		/* convert hourly transfer to hourly rate if needed */
		if (rate) {
			if ((ic->current-hourdata[i].date) > 3600) {
				hourdata[i].rx = hourdata[i].rx / ratediv;
				hourdata[i].tx = hourdata[i].tx / ratediv;
			} else {
				/* scale ongoing hour properly */
				if (chour != i) {
					hourdata[i].rx = hourdata[i].rx / ratediv;
					hourdata[i].tx = hourdata[i].tx / ratediv;
				} else {
					d = localtime(&ic->current);
					diff = d->tm_min * 60;
					if (!diff) {
						diff = 60;
					}
					if (cfg.rateunit == 1) {
						hourdata[i].rx = hourdata[i].rx * 8 / (float)diff;
						hourdata[i].tx = hourdata[i].tx * 8 / (float)diff;
					} else {
						hourdata[i].rx = hourdata[i].rx / (float)diff;
						hourdata[i].tx = hourdata[i].tx / (float)diff;
					}
				}
			}
		}

		if (hourdata[i].date >= hourdata[tmax].date) {
			tmax = i;
		}
		if (hourdata[i].rx >= max) {
			max = hourdata[i].rx;
		}
		if (hourdata[i].tx >= max) {
			max = hourdata[i].tx;
		}
	}

	/* scale values */
	scaleunit = getscale(max);
	if (max / scaleunit > 4) {
		step = 2;
	} else {
		step = 1;
	}

	for (i=step; (uint64_t)(scaleunit*i) <= max; i=i+step) {
		s = 121 * ((scaleunit * i) / (float)max);
		gdImageLine(ic->im, x+36, y+124-s, x+460, y+124-s, ic->cline);
		gdImageLine(ic->im, x+36, y+124-((s+prev)/2), x+460, y+124-((s+prev)/2), ic->clinel);
		gdImageString(ic->im, gdFontGetTiny(), x+16, y+121-s, (unsigned char*)getimagevalue(scaleunit*i, 3, rate), ic->ctext);
		prev = s;
	}
	s = 121 * ((scaleunit * i) / (float)max);
	if ( ((s+prev)/2) <= 128 ) {
		gdImageLine(ic->im, x+36, y+124-((s+prev)/2), x+460, y+124-((s+prev)/2), ic->clinel);
	}

	/* scale text */
	if (rate) {
		gdImageStringUp(ic->im, gdFontGetTiny(), x-2, y+70, (unsigned char*)getimagescale(scaleunit * (i - step), 1), ic->ctext);
	} else {
		gdImageStringUp(ic->im, gdFontGetTiny(), x-2, y+60, (unsigned char*)getimagescale(scaleunit * (i - step), 0), ic->ctext);
	}

	/* x-axis values and poles */
	for (i = 0; i < 24; i++) {
		s = tmax-i;
		if (s<0) {
			s+=24;
		}
		snprintf(buffer, 32, "%02d ", s);
		gdImageString(ic->im, gdFontGetTiny(), x+440-(i*17), y+128, (unsigned char*)buffer, ic->ctext);
		drawpole(ic, x+438-(i*17), y, 124, hourdata[s].rx, hourdata[s].tx, max);
	}

	/* axis */
	gdImageLine(ic->im, x+36-4, y+124, x+466, y+124, ic->ctext);
	gdImageLine(ic->im, x+36, y-10, x+36, y+124+4, ic->ctext);

	/* arrows */
	gdImageLine(ic->im, x+465, y+124, x+462, y+122, ic->ctext);
	gdImageLine(ic->im, x+465, y+124, x+462, y+126, ic->ctext);
	gdImageLine(ic->im, x+462, y+122, x+462, y+126, ic->ctext);
	gdImageLine(ic->im, x+36, y-9, x+38, y-6, ic->ctext);
	gdImageLine(ic->im, x+36, y-9, x+34, y-6, ic->ctext);
	gdImageLine(ic->im, x+34, y-6, x+38, y-6, ic->ctext);
}

void drawhourly(IMAGECONTENT *ic, const int rate)
{
	int width, height, headermod;
	char buffer[512];

	width = 500;
	height = 200;

	if (!ic->showheader) {
		headermod = 26;
		height -= 22;
	} else {
		headermod = 0;
	}

	ic->im = gdImageCreate(width, height);

	colorinit(ic);

	if (strcmp(ic->interface.name, ic->interface.alias) == 0 || strlen(ic->interface.alias) == 0) {
		snprintf(buffer, 512, "%s / hourly", ic->interface.name);
	} else {
		snprintf(buffer, 512, "%s (%s) / hourly", ic->interface.alias, ic->interface.name);
	}

	layoutinit(ic, buffer, width, height);
	drawlegend(ic, 242, 183-headermod);
	drawhours(ic, 12, 46-headermod, rate);
}

void drawlist(IMAGECONTENT *ic, const char *listname)
{
	int limit, listtype, textx, texty, offsetx = 0, offsety = 0;
	int width, height, headermod, i = 1;
	uint64_t e_rx, e_tx, e_secs, div, mult;
	char buffer[512], datebuff[16], stampformat[64];
	char titlename[8], colname[8];
	struct tm *d;
	dbdatalist *datalist = NULL, *datalist_i = NULL;
	dbdatalistinfo datainfo;

	if (strcmp(listname, "day") == 0) {
		listtype = 1;
		strncpy_nt(colname, listname, 8);
		snprintf(titlename, 8, "daily");
		strncpy_nt(stampformat, cfg.dformat, 64);
		limit = cfg.listdays;
	} else if (strcmp(listname, "month") == 0) {
		listtype = 2;
		strncpy_nt(colname, listname, 8);
		snprintf(titlename, 8, "monthly");
		strncpy_nt(stampformat, cfg.mformat, 64);
		limit = cfg.listmonths;
	} else if (strcmp(listname, "year") == 0) {
		listtype = 3;
		strncpy_nt(colname, listname, 8);
		snprintf(titlename, 8, "yearly");
		strncpy_nt(stampformat, "%Y", 64);
		limit = cfg.listyears;
	} else if (strcmp(listname, "top") == 0) {
		listtype = 4;
		snprintf(colname, 8, "day");
		strncpy_nt(stampformat, cfg.tformat, 64);
		limit = cfg.listtop;
		offsetx = 30;
		offsety = -16;
	} else {
		return;
	}

	if (limit == 0) {
		limit = -1;
	}

	if (!db_getdata(&datalist, &datainfo, ic->interface.name, listname, limit)) {
		printf("Error: Failed to fetch %s data.\n", "day");
		return;
	}

	if (listtype == 4) {
		if (limit > 0 && datainfo.count < (uint32_t)limit) {
			limit = datainfo.count;
		}
		if (limit <= 0 || datainfo.count > 999) {
			snprintf(titlename, 8, "top");
		} else {
			snprintf(titlename, 8, "top %d", limit);
		}
	}

	datalist_i = datalist;

	width = 500;
	if (listtype == 4) {
		height = 86;
	} else {
		height = 98;
	}
	height += 12 * datainfo.count;

	if (!ic->showheader) {
		headermod = 26;
		height -= 22;
	} else {
		headermod = 0;
	}

	ic->im = gdImageCreate(width, height);

	colorinit(ic);

	if (strcmp(ic->interface.name, ic->interface.alias) == 0 || strlen(ic->interface.alias) == 0) {
		snprintf(buffer, 512, "%s / %s", ic->interface.name, titlename);
	} else {
		snprintf(buffer, 512, "%s (%s) / %s", ic->interface.alias, ic->interface.name, titlename);
	}

	layoutinit(ic, buffer, width, height);

	if (datainfo.count) {
		if (listtype == 4) { // top
			if (cfg.ostyle<=2) {
				drawlegend(ic, 398, 40-headermod);
			}
		} else { // everything else
			if (cfg.ostyle>2) {
				drawlegend(ic, 432, 40-headermod);
			} else {
				drawlegend(ic, 385, 40-headermod);
			}
		}
	}

	textx = 10;
	texty = 40-headermod;

	if (listtype == 4) { // top
		snprintf(buffer, 512, "   #      day        rx           tx          total");
	} else { // everything else
		snprintf(buffer, 512, " %8s       rx           tx          total", colname);
	}
	if (cfg.ostyle>2) {
		strcat(buffer, "       avg. rate");
		gdImageString(ic->im, gdFontGetSmall(), textx, texty, (unsigned char*)buffer, ic->ctext);
		gdImageLine(ic->im, textx+2, texty+16, textx+392+offsetx, texty+16, ic->cline);
		gdImageLine(ic->im, textx+300+offsetx, texty+2, textx+300+offsetx, texty+40+offsety+(12*datainfo.count), ic->cline);
	} else {
		gdImageString(ic->im, gdFontGetSmall(), textx, texty, (unsigned char*)buffer, ic->ctext);
		gdImageLine(ic->im, textx+2, texty+16, textx+296+offsetx, texty+16, ic->cline);
	}
	gdImageLine(ic->im, textx+144+offsetx, texty+2, textx+144+offsetx, texty+40+offsety+(12*datainfo.count), ic->cline);
	gdImageLine(ic->im, textx+222+offsetx, texty+2, textx+222+offsetx, texty+40+offsety+(12*datainfo.count), ic->cline);

	texty += 20;

	while (datalist_i != NULL) {

		d = localtime(&datalist_i->timestamp);
		if (listtype == 4) {
			if (strftime(datebuff, 16, stampformat, d)<=8) {
				snprintf(buffer, 32, "  %2d   %*s   ", i, getpadding(8, datebuff), datebuff);
			} else {
				snprintf(buffer, 32, "  %2d  %-*s ", i, getpadding(11, datebuff), datebuff);
			}
		} else {
			if (strftime(datebuff, 16, stampformat, d)<=8) {
				snprintf(buffer, 32, "  %*s   ", getpadding(8, datebuff), datebuff);
			} else {
				snprintf(buffer, 32, " %-*s ", getpadding(11, datebuff), datebuff);
			}
		}
		strncat(buffer, getvalue(datalist_i->rx, 10, 1), 32);
		strcat(buffer, "   ");
		strncat(buffer, getvalue(datalist_i->tx, 10, 1), 32);
		strcat(buffer, "   ");
		strncat(buffer, getvalue(datalist_i->rx+datalist_i->tx, 10, 1), 32);
		if (cfg.ostyle>2) {
			strcat(buffer, "  ");
			if (datalist_i->next == NULL) {
				d = localtime(&ic->interface.updated);
				if (listtype == 1) { // day
					e_secs = d->tm_sec+(d->tm_min*60)+(d->tm_hour*3600);
				} else if (listtype == 2) { // month
					e_secs = mosecs(datalist_i->timestamp, ic->interface.updated);
				} else if (listtype == 3) { // year
					e_secs = d->tm_yday*86400+d->tm_sec+(d->tm_min*60)+(d->tm_hour*3600);
				} else if (listtype == 4) { // top
					e_secs = 86400;
				}
			} else {
				if (listtype == 1 || listtype == 4) { // day
					e_secs = 86400;
				} else if (listtype == 2) { // month
					e_secs = dmonth(d->tm_mon) * 86400;
				} else if (listtype == 3) { // year
					e_secs = (365 + isleapyear(d->tm_year+1900)) * 86400;
				}
			}
			strncat(buffer, gettrafficrate(datalist_i->rx+datalist_i->tx, e_secs, 14), 32);
		}
		gdImageString(ic->im, gdFontGetSmall(), textx, texty, (unsigned char*)buffer, ic->ctext);
		if (listtype == 4) { // top
			if (cfg.ostyle>2) {
				drawbar(ic, textx+428, texty+4, 52, datalist_i->rx, datalist_i->tx, datainfo.max);
			} else {
				drawbar(ic, textx+336, texty+4, 140, datalist_i->rx, datalist_i->tx, datainfo.max);
			}
		} else { // everything else
			if (cfg.ostyle>2) {
				drawbar(ic, textx+400, texty+4, 78, datalist_i->rx, datalist_i->tx, datainfo.max);
			} else {
				drawbar(ic, textx+304, texty+4, 170, datalist_i->rx, datalist_i->tx, datainfo.max);
			}
		}
		texty += 12;
		if (datalist_i->next == NULL) {
			break;
		}
		datalist_i = datalist_i->next;
		i++;
	}

	if (!datainfo.count) {
		gdImageString(ic->im, gdFontGetSmall(), textx, texty, (unsigned char*)"                 no data available", ic->ctext);
		texty += 12;
	}

	if (cfg.ostyle>2) {
		gdImageLine(ic->im, textx+2, texty+5, textx+392+offsetx, texty+5, ic->cline);
	} else {
		gdImageLine(ic->im, textx+2, texty+5, textx+296+offsetx, texty+5, ic->cline);
	}

	if (datainfo.count > 0 && listtype != 4) {

		d = localtime(&ic->interface.updated);
		if ( datalist_i->rx==0 || datalist_i->tx==0 ) {
			e_rx = e_tx = 0;
		} else {
			div = 0;
			mult = 0;
			if (listtype == 1) { // day
				div = d->tm_hour * 60 + d->tm_min;
				mult = 1440;
			} else if (listtype == 2) { // month
				div = mosecs(datalist_i->timestamp, ic->interface.updated);
				mult = dmonth(d->tm_mon) * 86400;
			} else if (listtype == 3) { // year
				div = d->tm_yday * 1440 + d->tm_hour * 60 + d->tm_min;
				mult = 1440 * (365 + isleapyear(d->tm_year + 1900));
			}
			if (div > 0) {
				e_rx = ((datalist_i->rx) / (float)div) * mult;
				e_tx = ((datalist_i->tx) / (float)div) * mult;
			} else {
				e_rx = e_tx = 0;
			}
		}
		snprintf(buffer, 32, " estimated   ");
		strncat(buffer, getvalue(e_rx, 10, 2), 32);
		strcat(buffer, "   ");
		strncat(buffer, getvalue(e_tx, 10, 2), 32);
		strcat(buffer, "   ");
		strncat(buffer, getvalue(e_rx+e_tx, 10, 2), 32);

		gdImageString(ic->im, gdFontGetSmall(), textx, texty+8, (unsigned char*)buffer, ic->ctext);
	}

	dbdatalistfree(&datalist);
}

void hextorgb(char *input, int *rgb)
{
	int offset;
	char hex[3], dec[4];

	if (input[0] == '#') {
		offset = 1;
	} else {
		offset = 0;
	}

	snprintf(hex, 3, "%c%c", input[(0+offset)], input[(1+offset)]);
	snprintf(dec, 4, "%d", (int)strtol(hex, NULL, 16));
	rgb[0] = atoi(dec);
	snprintf(hex, 3, "%c%c", input[(2+offset)], input[(3+offset)]);
	snprintf(dec, 4, "%d", (int)strtol(hex, NULL, 16));
	rgb[1] = atoi(dec);
	snprintf(hex, 3, "%c%c", input[(4+offset)], input[(5+offset)]);
	snprintf(dec, 4, "%d", (int)strtol(hex, NULL, 16));
	rgb[2] = atoi(dec);

	if (debug) {
		printf("%s -> %d, %d, %d\n", input, rgb[0], rgb[1], rgb[2]);
	}
}

void modcolor(int *rgb, const int offset, const int force)
{
	int i, overflow = 0;

	if (debug) {
		printf("m%d (%d): %d, %d, %d -> ", offset, force, rgb[0], rgb[1], rgb[2]);
	}

	for (i=0; i<3; i++) {
		if ((rgb[i]+offset)>255 || (rgb[i]+offset)<0) {
			overflow++;
		}
	}

	/* positive offset gives lighter color, negative darker if forced */
	/* otherwise the direction is changed depending on possible overflows */
	for (i=0; i<3; i++) {
		if (overflow<2 || force) {
			if ((rgb[i]+offset)>255) {
				rgb[i] = 255;
			} else if ((rgb[i]+offset)<0) {
				rgb[i] = 0;
			} else {
				rgb[i] += offset;
			}
		} else {
			if ((rgb[i]-offset)<0) {
				rgb[i] = 0;
			} else if ((rgb[i]-offset)>255) {
				rgb[i] = 255;
			} else {
				rgb[i] -= offset;
			}
		}
	}

	if (debug) {
		printf("%d, %d, %d\n", rgb[0], rgb[1], rgb[2]);
	}
}

char *getimagevalue(const uint64_t b, const int len, const int rate)
{
	static char buffer[64];
	int declen=0;

	if (b==0){
		snprintf(buffer, 64, "%*s", len, "--");
	} else {
		/* try to figure out what unit to use */
		if (rate) {
			if (b>=1000000000000) { /* 1000*1000*1000*1000 - value >=1000 Gbps -> show in Tbps */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1000000000000); /* 1000*1000*1000*1000 */
			} else if (b>=1000000000) { /* 1000*1000*1000 - value >=1000 Mbps -> show in Gbps */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1000000000); /* 1000*1000*1000 */
			} else if (b>=1000000) { /* 1000*1000 - value >=1000 Kbps -> show in Mbps */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1000000); /* 1000*1000 */
			} else if (b>=1000) { /* 1000 - value >=1000 bps -> show in Kbps */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1000);
			} else { /* bps by default */
				snprintf(buffer, 64, "%*"PRIu64"", len, b);
			}
		} else {
			if (b>=1073741824000) { /* 1024*1024*1024*1000 - value >=1000 GiB -> show in TiB */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1099511627776); /* 1024*1024*1024*1024 */
			} else if (b>=1048576000) { /* 1024*1024*1000 - value >=1000 MiB -> show in GiB */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1073741824); /* 1024*1024*1024 */
			} else if (b>=1024000) { /* 1024*1000 - value >=1000 KiB -> show in MiB */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1048576); /* 1024*1024 */
			} else if (b>=1000) { /* 1000 - value >=1000 B -> show in KiB */
				snprintf(buffer, 64, "%*.*f", len, declen, b/(float)1024);
			} else { /* B by default */
				snprintf(buffer, 64, "%*"PRIu64"", len, b);
			}
		}
	}

	return buffer;
}

char *getimagescale(const uint64_t b, const int rate)
{
	static char buffer[8];
	int unit = cfg.unitmode;
	int div = 1, p = 1024;

	if (b==0) {
		snprintf(buffer, 8, "--");
	} else {
		if (rate) {
			if (cfg.rateunit) {
				p = 1000;
				unit = 2;
			}
			while (div < UNITPREFIXCOUNT && b >= (pow(p, div-1) * 1000)) {
				div++;
			}
			snprintf(buffer, 8, "%s", getrateunitprefix(unit, div));
		} else {
			while (div < UNITPREFIXCOUNT && b >= (pow(p, div-1) * 1000)) {
				div++;
			}
			snprintf(buffer, 8, "%s", getunitprefix(div));
		}
	}
	return buffer;
}

uint64_t getscale(const uint64_t input)
{
	int i;
	unsigned int div = 1024;
	uint64_t result = input;

	if (cfg.hourlyrate) {
		div = 1000;
	}

	/* get unit */
	for (i=0; result>div; i++) {
		result = result / div;
	}

	/* round result depending of scale */
	if (result>300) {
		result = result/4 + (100 - ((result/4) % 100));
	} else if (result>20) {
		result = result/4 + (10 - ((result/4) % 10));
	} else {
		result = result/4;
	}

	/* put unit back */
	if (i) {
		result = result * pow(div, i);
	}

	/* make sure result isn't zero */
	if (!result) {
		result = pow(div, i);
	}

	return result;
}
