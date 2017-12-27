import {EventEmitter, Input, Output} from "@angular/core";
import {EventService, OBSERVER_ROI_SET} from "../../event.service";
import {CameraService} from "../../camera.service";
import {CarObserver} from "../race.service";

export abstract class AbstractObserverComponent {
    @Input() protected roiColor: string;
    @Input("observer")
    set theObserver(observer: CarObserver) {
        this.observer = observer;
        this.setRoi();
    }
    protected observer: CarObserver;
    protected max_width = 640;
    protected max_height = 480;
    @Output() onChange = new EventEmitter();

    constructor(public eventService: EventService,
                public cameraService: CameraService,
                public type: string) {
        cameraService.getCameraInfo().subscribe(cameraInfo => {
            this.max_width = cameraInfo.dimension.width;
            this.max_height = cameraInfo.dimension.height;
            this.updateRoi();
        });
    }

    protected updateObserver() {
        // this.observer.active =  (Number(this.observer.active));
        this.onChange.emit(this.observer);
    }

    activate() {
        if (this.observer) {
            this.observer.active = true;
            this.observer.condition_achieved = false;
            this.updateObserver();
        }
    }

    public deactivate() {
        if (this.observer) {
            this.observer.active = false;
            this.observer.condition_achieved = false;
            this.updateObserver();
        }
    }

    protected updateRoi() {
        this.setRoi();
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
            if (this.observer.roi.y + this. observer.roi.height > this.max_height) {
                this.observer.roi.height = this.max_height - this.observer.roi.y;
            }
            this.observer.roi.color = this.roiColor;
            this.observer.roi.type = this.type;
            this.eventService.emit(OBSERVER_ROI_SET, this.observer);
            this.updateObserver();
        }
    }

}