import React, { Component } from 'react';
import Dropzone from 'react-dropzone';

export default class ImageUpload extends Component {
	constructor(props) {
		super(props);

		this.state = {
			files: []
		}
	}

	consume = (stream, total = 0) => {
		while (stream.state === "readable") {
		  var data = stream.read()
		  total += data.byteLength;
		  console.log("received " + data.byteLength + " bytes (" + total + " bytes in total).")
		}
		if (stream.state === "waiting") {
		  stream.ready.then(() => this.consume(stream, total))
		}
		return stream.closed
	}

	handleDrop = (accepted, rejected) => {
		console.log("Accepted", accepted);
		console.log("Rejected", rejected);
		let me = this;

		var valid_types = ["image/jpeg", "image/png"];
		let size = accepted.length;
		let i = 0;
		let files = this.state.files;
		
		accepted.forEach(file => {

			var file_type = file["type"];
			++i;

			if (valid_types.includes(file_type)) {
				const reader = new FileReader();
				reader.onload = (e) => {
					let b64 = e.target.result;
					files.push(b64);
					
					var canvas = document.getElementById('myCanvas');
					var context = canvas.getContext('2d');
					var imageObj = new Image();
			  
					imageObj.onload = function() {
						let m = Math.min(imageObj.width, imageObj.height);
						
						// draw cropped image
						var sourceX = 0;
						var sourceY = 0;
				
						context.drawImage(imageObj, sourceX, sourceY, m, m, 0, 0, canvas.width, canvas.height);
						let dataUrl = canvas.toDataURL('image/jpeg');
						
						let p = b64.indexOf(",");
						b64 = b64.substr(p+1);

						p = dataUrl.indexOf(",");
						dataUrl = dataUrl.substr(p+1);

						let payload = b64 + ";" + dataUrl;

						fetch(me.props.domain + "image", {
							method: "POST",
							body: payload
						})
						.then((e) => {
							if (e.status >= 300)
								throw "Invalid";
				
							return e.text();
						})
						.then((e) => {
							console.log(e);
							me.setState({processing: false, r_error: true, files: []});
							me.props.fetchMyImages();
							me.props.hideImageUpload();
						}).catch((e) => {
							console.log(e);
							
							me.setState({processing: false, r_error: true});
						});
					};
					imageObj.src=b64;


					/*fetch(this.domain + "image", {
						method: "POST",
						body: b64
					})
					.then((e) => {
						if (e.status >= 300)
							throw "Invalid";
			
						return e.text();
					})
					.then((e) => {
						console.log(e);
						
						this.setState({processing: false, r_error: false});
						this.props.setUserInfo({
							username: this.state.s_username
						});
						this.hide();
					}).catch((e) => {
						console.log(e);
						
						this.setState({processing: false, r_error: true});
					});*/
					
					if (i === size) {
						me.setState({files: files});
					}
				};
				reader.onabort = () => console.log('file reading was aborted');
				reader.onerror = () => console.log('file reading has failed');
		
				reader.readAsDataURL(file);
			}
		});
	}

	render() {
		return (
			<div className={`image-upload${this.props.shown ? "" : " hide"}`}>
				<canvas id="myCanvas"></canvas>
				<div className="close" onClick={this.props.hideImageUpload}></div>
				<Dropzone className="uploadfield" onDrop={this.handleDrop}>
					{this.state.files.length === 0 && <div id="middle">Drag and Drop files here, or click the area, to upload.</div>}
					{this.state.files.length > 0 &&
						<React.Fragment>
							<div className="msg">Drag and Drop files here, or click the area, to upload.</div>
							<div className="gallery">
								{
									this.state.files.map((e, i) => {
										return <div key={i} style={{backgroundImage: "url("+e+")"}}></div>; // backgroundImage: "url("+")"
									})
								}
							</div>
						</React.Fragment>
					}
					{/*id="middle" <span className="success">Images uploaded!</span>*/}
					{/*<span className="fail">Could not upload!</span>*/}
				</Dropzone>
            </div>
		);
	}
}