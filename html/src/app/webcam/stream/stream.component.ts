import {
    AfterViewInit, Component, ElementRef, EventEmitter, HostListener, Input, Output, ViewChild
} from '@angular/core';
import { Roi } from '../../observers/observer.service';
import {CameraService, CameraSettings} from "../../camera.service";
import {EventService, ROI_CLEAR, ROI_LOADED, ROI_SET} from "../../event.service";

interface Point {
    x: number;
    y: number;
}

@Component({
    selector: 'stream',
    template: require('./stream.html'),
    styles: [require('./stream.scss')]
})
export class StreamComponent {
    private reloadInterval: any;
    private last_update: Date;
    private status: string = 'loading';
    private cameraSettings: CameraSettings;
    private interval: number = 5 * 1000;
    @Input('roi')
    set theRoi(roi: Roi) {
        // console.log(roi);
        if (roi) {
            this.roi = roi;
            this.setRoi();
        }
    }
    private roi: Roi;
    @ViewChild('roi') roiElem;
    @Output('onChange') changeEmitter = new EventEmitter();
    private streamDefault = '/assets/img/testbeeld.png';
    private streamUrl = this.streamDefault;
    private mouseDown = false;

    private rois: any[] = [];
    @HostListener('mousedown', ['$event'])
    onMouseDown(event: any) {
        this.mouseDown = true;
        const point = this.getPoint(event);
        this.roi = {
            x: point.x,
            y: point.y,
            width: 0,
            height: 0
        };
        this.setRoi();
    }

    @HostListener('mouseup', ['$event'])
    onMouseUp(event: any) {
        this.mouseDown = false;
        this.changeEmitter.emit(this.roi);
    }

    @HostListener('mousemove', ['$event'])
    onMouseMove(event: any) {
        if (this.mouseDown) {
            const point = this.getPoint(event);
            this.roi.width  = point.x - this.roi.x;
            this.roi.height = point.y - this.roi.y;
            this.setRoi();
        }
    }

    @HostListener('ondragstart', ['$event'])
    onDragstart(event: any) {
        return false;
    }

    constructor(private cameraService: CameraService,
                private eventService: EventService) {
        this.cameraService.getCameraInfo().subscribe(cameraSettings => {
            if (cameraSettings) {
                this.cameraSettings = cameraSettings;
                this.interval = cameraSettings.previewDelay;
                if (this.reloadInterval) {
                    setTimeout(() => {
                        clearInterval(this.reloadInterval);
                        this.getWebcamImage();
                        this.startReload();
                    }, 300);
                }
            }
        });
        this.eventService.subscribe(event => {
            if (event.name === ROI_SET) {
                const newRoi = event.data;
                this.rois = this.rois.filter(roi => roi.type !== newRoi.type);
                this.rois.push(newRoi);
            } else if (event.name === ROI_LOADED) {
                this.rois.push(event.data);
            } else if (event.name === ROI_CLEAR) {
                this.rois = [];
            }
        })
    }

    ngAfterViewInit() {
        this.getWebcamImage();
        this.startReload();
    }

    private setRoi() {
        if (this.roiElem) {
            this.roiElem.nativeElement.style.top    = this.roi.y + 'px';
            this.roiElem.nativeElement.style.height = this.roi.height + 'px';
            this.roiElem.nativeElement.style.left   = this.roi.x + 'px';
            this.roiElem.nativeElement.style.width  = this.roi.width + 'px';
        }
    }

    private getPoint(event: any): Point {
        const ev = event || window.event; //Moz || IE
        const point = { x: 0, y: 0};
        if (ev.pageX) { //Moz
            point.x = ev.layerX;
            point.y = ev.layerY;
        } else if (ev.clientX) { //IE
            point.x = ev.clientX + document.body.scrollLeft;
            point.y = ev.clientY + document.body.scrollTop;
        }
        return point;
    }

    private startReload() {
        this.reloadInterval = setInterval(() => {
            this.getWebcamImage();
        }, (this.interval));
    }

    private getWebcamImage() {
        this.last_update = new Date();
        const url = '/camera/snapshot/' + this.last_update;
        const img = new Image();
        img.onload = () => {
            this.status = 'loaded';
            this.streamUrl = url;
        };
        img.onerror = () => {
            this.status = 'error';
            this.streamUrl = this.streamDefault;
        };
        img.src = url;
    }
}