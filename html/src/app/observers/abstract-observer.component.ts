
import {Input} from "@angular/core";
import {CarObserver, ObserverService} from "./observer.service";
import {EventService, ROI_LOADED, ROI_SET} from "../event.service";
import {CameraService} from "../camera.service";

export abstract class AbstractObserverComponent {
    @Input() protected roiColor: string;
    protected observer: CarObserver;
    protected max_width = 640;
    protected max_height = 480;

    constructor(public observerService: ObserverService,
                public eventService: EventService,
                public cameraService: CameraService,
                public type: string) {
        cameraService.getCameraInfo().subscribe(cameraInfo => {
            this.max_width = cameraInfo.dimension.width;
            this.max_height = cameraInfo.dimension.height;
        });
    }

    protected loadObserver() {
        this.observerService.getObserver(this.type).subscribe(data => {
            this.observer = data;
            this.deactivate();
            const roi: any = this.observer.roi;
            roi.color = this.roiColor;
            roi.type = this.type;
            this.eventService.emit(ROI_LOADED, roi);
        });
    }

    protected updateObserver() {
        this.observer.active =  (Number(this.observer.active));
        this.observerService.saveObserver(this.type, this.observer).subscribe(observer => this.observer = observer);
    }

    activate() {
        if (this.observer) {
            this.observer.active = 1;
            this.observer.condition_achieved = 0;
            this.updateObserver();
        }
    }

    public deactivate() {
        if (this.observer) {
            this.observer.active = 0;
            this.observer.condition_achieved = 0;
            this.updateObserver();
        }
    }

    protected setRoi() {
        if (this.observer) {
            if (this.observer.roi.x > this.max_width) {
                this.observer.roi.x = this.max_width;
            }
            if (this.observer.roi.x + this.observer.roi.width > this.max_width) {
                this.observer.roi.width = this.max_width - this.observer.roi.x;
            }
            if (this.observer.roi.y > this.max_height) {
                this.observer.roi.y = this.max_height;
            }
            if (this.observer.roi.y + this.observer.roi.height > this.max_height) {
                this.observer.roi.height = this.max_height - this.observer.roi.y;
            }
        }
        const roi: any = this.observer.roi;
        roi.color = this.roiColor;
        roi.type = this.type;
        this.eventService.emit(ROI_SET, roi);
        this.updateObserver();
    }

}