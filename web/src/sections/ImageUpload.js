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
					files.push(e.target.result);

					console.log(e.target.result);

					/*fetch("/music/pk/altes-kamuffel.flac")
						.then(res => this.consume(res.body))
						.then(() => console.log("consumed the entire body without keeping the whole thing in memory!"))
						.catch((e) => console.error("something went wrong", e))*/
					
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