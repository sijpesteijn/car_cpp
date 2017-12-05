
import {EventEmitter, Input, Output} from "@angular/core";
import {CarObserver, ObserverService} from "./observer.service";
import {EventService, ROI_LOADED, ROI_SET} from "../event.service";
import {CameraService} from "../camera.service";

export abstract class AbstractObserverComponent {
    @Input() protected roiColor: string;
    @Input()
    set observer(observer: CarObserver) {
        this._observer = observer;
    }
    protected _observer: CarObserver;
    protected max_width = 640;
    protected max_height = 480;
    @Output() onChange = new EventEmitter();

    constructor(public observerService: ObserverService,
                public eventService: EventService,
                public cameraService: CameraService,
                public type: string) {
        cameraService.getCameraInfo().subscribe(cameraInfo => {
            this.max_width = cameraInfo.dimension.width;
            this.max_height = cameraInfo.dimension.height;
        });
    }

    // protected loadObserver() {
    //     this.observerService.getObserver(this.type).subscribe(data => {
    //         this.observer = data;
    //         this.deactivate();
    //         const roi: any = this.observer.roi;
    //         roi.color = this.roiColor;
    //         roi.type = this.type;
    //         this.eventService.emit(ROI_LOADED, roi);
    //     });
    // }

    protected updateObserver() {
        this._observer.active =  (Number(this._observer.active));
        this.onChange.emit(this._observer);
        // this.observerService.saveObserver(this.type, this.observer).subscribe(observer => this.observer = observer);
    }

    activate() {
        if (this._observer) {
            this._observer.active = 1;
            this._observer.condition_achieved = 0;
            this.updateObserver();
        }
    }

    public deactivate() {
        if (this._observer) {
            this._observer.active = 0;
            this._observer.condition_achieved = 0;
            this.updateObserver();
        }
    }

    protected setRoi() {
        if (this._observer) {
            if (this._observer.roi.x > this.max_width) {
                this._observer.roi.x = this.max_width;
            }
            if (this._observer.roi.x + this._observer.roi.width > this.max_width) {
                this._observer.roi.width = this.max_width - this._observer.roi.x;
            }
            if (this._observer.roi.y > this.max_height) {
                this._observer.roi.y = this.max_height;
            }
            if (this._observer.roi.y + this. _observer.roi.height > this.max_height) {
                this._observer.roi.height = this.max_height - this._observer.roi.y;
            }
        }
        const roi: any = this._observer.roi;
        roi.color = this.roiColor;
        roi.type = this.type;
        this.eventService.emit(ROI_SET, roi);
        this.updateObserver();
    }

}