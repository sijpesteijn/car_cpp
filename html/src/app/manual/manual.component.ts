import { Component } from '@angular/core';
import { Car, CarService } from "../car.service";
import { Config } from "../app.config";

@Component({
    selector: 'manual',
    template: require('./manual.html'),
    styles: [require('./manual.scss')]
})
export class ManualComponent {
    private car: Car;
    private streamUrl: string = '/assets/img/testbeeld.png';

    constructor(private carService: CarService, private config: Config) {
        this.streamUrl = config.get('stream');
    }

    ngOnInit() {
        this.carService.getCar().subscribe(car => this.car = car);

        // setInterval(() => {
            // if (this.changed) {
            //     this.carService.saveCar(this.car).subscribe(() => this.changed = false);
            // }
        // }, 200);
    }

    onChange(event: any) {
        this.car.throttle = event.deltaY;
        this.car.angle = event.deltaX;
        // this.carService.saveCar(this.car).subscribe();
    }
}